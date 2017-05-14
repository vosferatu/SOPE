#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

FILE* balFile;

int capacidade; //capacidade  da sauna

char GENERO_ATUAL = 'E'; //genero quando a sauna esta vazia
int PEDIDOS_POR_LER; //numero de pedidos ainda por ler
int LUGARES_OCUPADOS = 0; //numero de pessoas dentro da sauna

int ENTRADA_FIFO_FD; //file descriptor do FIFO de entrada
int REJEITADOS_FIFO_FD; //file descriptor do FIFO de rejeitados

struct timeval abertura; //struct que guarda a hora de inicio do programa

pthread_t threadsTid[255]; //array com os tids das threads a correr
int threadPos=0;

int M_SERVIDOS=0;
int M_REJEITADOS=0;
int M_RECEBIDOS=0;
int F_SERVIDOS=0;
int F_REJEITADOS=0;
int F_RECEBIDOS=0;

typedef struct {
    int id; //numero do pedido
    char genero;
    int duracao;
    int recusas;
} Pedido;

void printStats(){

    printf("[SAUNA RECEBIDOS]\n");
    printf(" > m: %d\n", M_RECEBIDOS);
    printf(" > f: %d\n", F_RECEBIDOS);
    printf(" > total: %d\n", M_RECEBIDOS + F_RECEBIDOS);

    printf("[SAUNA SERVIDOS]\n");
    printf(" > m: %d\n", M_SERVIDOS);
    printf(" > f: %d\n", F_SERVIDOS);
    printf(" > total: %d\n", M_SERVIDOS + F_SERVIDOS);

    printf("[SAUNA REJEITADOS]\n");
    printf(" > m: %d\n", M_REJEITADOS);
    printf(" > f: %d\n", F_REJEITADOS);
    printf(" > total: %d\n", M_REJEITADOS + F_REJEITADOS);
}

void escreverFicheiro(Pedido *pedido, int tid, char* tip){

    struct timeval end; //struct que guarda a hora do instante pretendido
    gettimeofday(&end, NULL);
    double inst = (double)(end.tv_usec - abertura.tv_usec)/100; //milissegundos depois do inicio do programa

    fprintf(balFile, "%-9.2f - %-4d - %-12d - %-4d: %-1c - %-4d - %-10s\n", inst, getpid(), tid ,pedido->id,pedido->genero, pedido->duracao, tip);

    if(pedido->genero=='M'){
        if(strcmp(tip,"REJEITADO")==0) M_REJEITADOS++;
        if(strcmp(tip,"RECEBIDO")==0) M_RECEBIDOS++;
        if(strcmp(tip,"SERVIDO")==0) M_SERVIDOS++;
    }
    else{
        if(strcmp(tip,"REJEITADO")==0) F_REJEITADOS++;
        if(strcmp(tip,"RECEBIDO")==0) F_RECEBIDOS++;
        if(strcmp(tip,"SERVIDO")==0) F_SERVIDOS++;
    }

}

void tratarRejeitados(Pedido* pedido){

    pedido->recusas = pedido->recusas + 1;

    if(pedido->recusas < 3)
    PEDIDOS_POR_LER++; //como foi rejeitado menos que 3 vezes quer dizer que eventualmente vai chegar à sauna novamente

    printf(". SAUNA (rejeitado): P:%i-G:%c-T:%i-D:%i;\n", pedido->id, pedido->genero, pedido->duracao, pedido->recusas);
    write(REJEITADOS_FIFO_FD, pedido, sizeof(Pedido));
}

void *atendimento(void *arg) {

    Pedido *pedido = (Pedido*)arg;

    printf(". SAUNA: %d entrou\n",pedido->id);
    printf("%d: %d\n", pedido->id, pedido->duracao);
    usleep(pedido->duracao*100);

    escreverFicheiro(pedido, pthread_self(),"SERVIDO");

    LUGARES_OCUPADOS--; //a pessoa sai

    printf(". SAUNA: %d saiu\n",pedido->id);

    printf(". SAUNA: PEOPLE IN SAUNA: %d\n", LUGARES_OCUPADOS);

    if(LUGARES_OCUPADOS==0){
        GENERO_ATUAL = 'E';
        printf(". SAUNA: allowed genero: %c\n",GENERO_ATUAL);
    }

    pthread_exit(NULL);
}

int validarPedidos(Pedido *pedido) {

    if(pedido->genero != GENERO_ATUAL) //se o genero for diferente do correntemente na sauna
    return 0;
    else if (LUGARES_OCUPADOS >= capacidade) //se a sauna estiver cheia
    return 0;
    else return 1;
}

void tratarPedidos(Pedido* pedido){

    if(GENERO_ATUAL=='E') { //primeira pessoa na sauna
        GENERO_ATUAL = pedido->genero; //o genero da sauna muda
        printf(". SAUNA: allowed genero: %c\n",GENERO_ATUAL);
        printf(". SAUNA (servido): P:%i-G:%c-T:%i-D:%i;\n", pedido->id, pedido->genero, pedido->duracao, pedido->recusas);
        LUGARES_OCUPADOS++; //numero de pessoas na sauna aumenta
        escreverFicheiro(pedido, getpid(),"RECEBIDO");
        pthread_create(&threadsTid[threadPos], NULL, atendimento,pedido);
        threadPos++;
    } else {
        if(validarPedidos(pedido) != 0) {
            printf(". SAUNA (servido): P:%i-G:%c-T:%i-D:%i;\n", pedido->id, pedido->genero, pedido->duracao, pedido->recusas);
            LUGARES_OCUPADOS++; //numero de pessoas na sauna aumenta
            escreverFicheiro(pedido, getpid(),"RECEBIDO");
            pthread_create(&threadsTid[threadPos], NULL, atendimento,pedido);
            threadPos++;
        } else {
            escreverFicheiro(pedido, getpid(),"RECEBIDO");
            escreverFicheiro(pedido, getpid(), "REJEITADO");
            tratarRejeitados(pedido);

        }
    }
    return;
}

void receberPedidos() {
    Pedido* pedido;
    int n;
    while(PEDIDOS_POR_LER != 0) { //enquanto houver pedidos para ler
        pedido = malloc(sizeof(Pedido));
        n=read(ENTRADA_FIFO_FD, pedido, sizeof(Pedido));
        if(n>0){
            printf(". SAUNA REQUESTS TO READ: %d\n", PEDIDOS_POR_LER);
            tratarPedidos(pedido);
            PEDIDOS_POR_LER--; //um pedido foi lido, decrementar pedidos para ler
        }
    }
    if(PEDIDOS_POR_LER==0){ //fim do pogramas
        Pedido* pedido =malloc(sizeof(Pedido));
        pedido->id=-1; //quando o gerador recebe um pedido com id -1, para de ler
        write(REJEITADOS_FIFO_FD,  pedido, sizeof(pedido));
        close(REJEITADOS_FIFO_FD); //fechar fifo de rejeitados
        close(ENTRADA_FIFO_FD); //fechar fifo de entrada
    }
    return;
}

int main(int argc, char* argv[]) {

    gettimeofday(&abertura, NULL); //guardar na struct a hora de inicio do programa

    //Tratamento de argumentos

    if (argc != 2) {
        printf("Numero invalido de argumentos! USO: sauna <n. max utentes>");
        exit(EXIT_FAILURE);
    }

    capacidade = atoi(argv[1]);


    //Criaçao do ficheiro de registo
    int pid;
    pid = getpid();
    char balPathname [20];
    sprintf (balPathname, "/tmp/bal.%d", pid);
    balFile=fopen(balPathname, "w");

    if(balFile == NULL)
    printf(". SAUNA: Error opening balFile\n");

    //Abertura FIFO rejeitados
    while ((ENTRADA_FIFO_FD = open("/tmp/entrada", O_RDONLY)) == -1) {
        if (errno == EEXIST)
        printf("FIFO 'entrada' não existe! Tentando de novo...\n");
    }

    //Criação e abertura de FIFO de rejeitados
    if (mkfifo("/tmp/rejeitados", S_IRUSR | S_IWUSR) != 0) {
        if (errno == EEXIST)
        {
            //Do nothing
        }
        else
        printf("Nao foi possivel criar o FIFO '/tmp/rejeitados'\n");
        exit(EXIT_FAILURE);
    }
    // >abertura
    while ((REJEITADOS_FIFO_FD = open("/tmp/rejeitados", O_WRONLY | O_NONBLOCK)) == -1) {
        printf("Abrindo 'rejeitados'...\n");
    }

    //Ler numero de pedidos
    read(ENTRADA_FIFO_FD, &PEDIDOS_POR_LER, sizeof(int));

    //Receber Pedidos
    receberPedidos();

    int k;
    for(k=0; k < 255; k++){
        pthread_join(threadsTid[k], NULL); //espera pelas threads que estão a correr
    }

    printStats();
    fclose(balFile);
    unlink("/tmp/rejeitados");

    exit(0);
}
