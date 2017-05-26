#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "uteis.h"

int ID = 1;
int TEMPO_MAX, NUM_PEDIDOS;
int PEDIDO_M = 0, PEDIDO_F = 0, REJEITADO_M = 0, REJEITADO_F = 0, DESCARTADO_M = 0, DESCARTADO_F = 0;

//FIFO and log files & descriptors

char* LOG_MSG_PATH;
FILE* LOG_FILE;

struct timeval inicio, stop;

char* tip[] = {"PEDIDO", "REJEITADO", "DESCARTADO"};

void criarFicheiroRegisto (){
    char pid_str[32], endString[64] = "/tmp/ger.";
    sprintf(pid_str, "%d", getpid());
    strcat(endString, pid_str);
    LOG_MSG_PATH = (char*) malloc(strlen(endString) + 1);
    strcpy(LOG_MSG_PATH, endString);
}

void escreverFicheiro(Pedido* pedido ){
    gettimeofday(&stop, NULL);
    float elapsed = (float) ((float) stop.tv_usec - inicio.tv_usec) / 1000;

    LOG_FILE = fopen(LOG_MSG_PATH, "a"); //Abre o Log file no modo "a" -- Appends to a file
    fprintf(LOG_FILE, "%4.2f - %4d - %2d - %c - %5d - %9s\n", elapsed, getpid(), pedido->id, pedido->genero, pedido->duracao, tip[0]);
    fclose(LOG_FILE); //Closes log file.
}

void* gerador(void* arg){

  if(mkfifo(FIFO_ENTRADAS, S_IRUSR | S_IWUSR) != 0 && errno != EEXIST){
    perror("Erro ao criar fifo de entradas");
    exit(-1);
  }

  while ((FD_ENTRADAS = open(FIFO_ENTRADAS, O_WRONLY)) == -1){
    if (errno != ENXIO){
      perror("Erro ao abrir o fifo de entradas");
      exit(-1);
    }
  }

  write(FD_ENTRADAS, &NUM_PEDIDOS, sizeof(int));

  for (int i = 0; i < NUM_PEDIDOS; i++){
    Pedido* pedido = malloc(sizeof(Pedido));

    pedido->id = ID++;
    pedido->genero = (rand() % 2) ? 'M' : 'F';
    pedido->duracao = rand() % TEMPO_MAX + 1;
    pedido->recusas = 0;

    if (pedido->genero == 'M') {
        PEDIDO_M++;
    }
    else PEDIDO_F++;

    escreverFicheiro(pedido);

    write(FD_ENTRADAS, pedido, sizeof(Pedido));

    free(pedido);
  }

  return NULL;
}

void* listenerRejeitados(void* arg){
  int fifo_fd;
  Pedido* r = malloc(sizeof(Pedido));

  while ((fifo_fd = open(FIFO_REJEITADOS, O_RDONLY)) == -1){
    if (errno == ENOENT) printf("Esperando...\n");
    sleep(1);
  }

  while(read(fifo_fd, r, sizeof(Pedido)) != 0){

    gettimeofday(&stop, NULL);
    float elapsed = (float) ((float) stop.tv_usec - inicio.tv_usec) / 1000;

    LOG_FILE = fopen(LOG_MSG_PATH, "a");
    fprintf(LOG_FILE, "%4.2f - %4d - %2d - %c - %5d - %9s\n", elapsed, getpid(), r->id, r->genero, r->duracao, tip[1]);
    fclose(LOG_FILE);

    if (r->genero == 'M') REJEITADO_M++;
    else REJEITADO_F++;

    if (r->recusas < 3){
      write(FD_ENTRADAS, r, sizeof(*r));
    } else {
      gettimeofday(&stop, NULL);
      float elapsed = (float) ((float) stop.tv_usec - inicio.tv_usec) / 1000;

      LOG_FILE = fopen(LOG_MSG_PATH, "a"); //Opens log file.
      fprintf(LOG_FILE, "%4.2f - %4d - %2d - %c - %5d - %9s\n", elapsed, getpid(), r->id, r->genero, r->duracao, tip[2]);
      fclose(LOG_FILE); //Closes log file.

      if (r->genero == 'M') DESCARTADO_M++;
      else if (r->genero == 'F') DESCARTADO_F++;
    }
    sleep(1);
  }
  free(r);

  return NULL;
}

int main(int argc, char* argv[]){

  gettimeofday(&inicio, NULL);

  time_t t;
  srand((unsigned) time(&t));

  if(argc != 3){
    printf("Argumentos invalidos. USO: gerador <n. pedidos> <max duracao>\n");
    exit(-1);
  }
  NUM_PEDIDOS = atoi(argv[1]);
  TEMPO_MAX = atoi(argv[2]);

  criarFicheiroRegisto();

  pthread_t tid_ent, tid_rej;

  pthread_create(&tid_ent, NULL, gerador, NULL);
  pthread_create(&tid_rej, NULL, listenerRejeitados, NULL);

  pthread_join(tid_ent, NULL);
  pthread_join(tid_rej, NULL);

  unlink(FIFO_ENTRADAS);

  pthread_exit(NULL);
}
