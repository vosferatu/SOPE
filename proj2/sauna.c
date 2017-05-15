#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include "uteis.h"

//Mutex initializer.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Sauna specific variables.
int capacidade, vagas;
char GENERO_ATUAL;
int RECEBIDO, REJEITADO_F, REJEITADO_M, SERVIDO_F, SERVIDO_M;
int PEDIDOS_EM_ESPERA;


//Log file path and descriptor.
char* LOG_MSG_PATH;
FILE* LOG_FILE;
char* tip[] = {"RECEBIDO", "REJEITADO", "SERVIDO"};
struct timeval inicio, stop; //estruturas para armazenar tempo

void imprimirEstatisticas(){

    printf("\n[RESUMO]\n\n");
    printf("RECEBIDOS:  %d PEDIDOS\n", RECEBIDO);
    printf("REJEITADOS: TOTAL %d, HOMENS %d, MULHERES %d\n", REJEITADO_M + REJEITADO_F, REJEITADO_M, REJEITADO_F);
    printf("SERVIDOS: TOTAL %d, HOMENS %d, MULHERES %d\n\n", SERVIDO_M + SERVIDO_F, SERVIDO_M, SERVIDO_F);
    printf("\n\n");
}

void criarFicheiroRegisto(){
    char pid_str[32], endString[64] = "/tmp/bal.";
    sprintf(pid_str, "%d", getpid());
    strcat(endString, pid_str);
    LOG_MSG_PATH = (char*) malloc(strlen(endString) + 1);
    strcpy(LOG_MSG_PATH, endString);
}

void escreverFicheiro(Pedido* r){
    gettimeofday(&stop, NULL);
    double elapsed = (stop.tv_sec - inicio.tv_sec)*1000.0f + (stop.tv_usec - inicio.tv_usec) / 1000.0f;
    LOG_FILE = fopen(LOG_MSG_PATH, "a");
    fprintf(LOG_FILE, "%9.2f - %4d - %15lu - %2d - %c - %5d - %8s\n", elapsed, getpid(), pthread_self(), r->id, r->genero, r->duracao, tip[2]);
    fclose(LOG_FILE);
}

void* atendimento(void* arg){

  Pedido* r = (Pedido*) arg;

  if (r->genero == 'M') {
      printf("M: entrou o utente %d por %d ms.\n", r->id, r->duracao);
      SERVIDO_M++;
  }
  else {
      printf("F: entrou a utente %d por %d ms.\n", r->id, r->duracao);
      SERVIDO_F++;
  }

  usleep(r->duracao * 1000);

  pthread_mutex_lock(&mutex);
  vagas++;
  if (vagas == capacidade){
     GENERO_ATUAL = 'V'; // a sauna encontra-se vazia
  }
  pthread_mutex_unlock(&mutex);

  printf("Utente %d saiu.\n", r->id);

  pthread_exit(NULL);
}

void* handlerPedidos(void* arg){
  pthread_t tid[128] = {0};
  int current = 0;
  int fifo_fd;

  while ((fifo_fd = open(FIFO_ENTRADAS, O_RDONLY)) == -1){
    if (errno == ENOENT || errno == ENXIO){
      printf("Retrying...\n");
      sleep(1);
    } else {
      perror("Error opening GENERATE fifo");
      exit(-1);
    }
  }

  //Gets the amount of requests to read.
  read(fifo_fd, &PEDIDOS_EM_ESPERA, sizeof(int));

  //Generates the specified amount of requests.
  while (PEDIDOS_EM_ESPERA){
    Pedido* r = malloc(sizeof(Pedido));

    RECEBIDO++;

    //Handles a new request.
    if (read(fifo_fd, r, sizeof(Pedido)) != 0){
      PEDIDOS_EM_ESPERA--;

      gettimeofday(&stop, NULL); //Stops counting time.
      double elapsed = (stop.tv_sec - inicio.tv_sec)*1000.0f + (stop.tv_usec - inicio.tv_usec) / 1000.0f;
      LOG_FILE = fopen(LOG_MSG_PATH, "a"); //Opens log file.
      fprintf(LOG_FILE, "%9.2f - %4d - %15lu - %2d - %c - %5d - %8s\n", elapsed, getpid(), pthread_self(), r->id, r->genero, r->duracao, tip[0]);
      fclose(LOG_FILE); //Closes log file.

      if ((r->genero == GENERO_ATUAL || GENERO_ATUAL == 'V') && vagas > 0){
        vagas--; //Decrements the available seat counter.
        GENERO_ATUAL = r->genero;
        pthread_create(&tid[current], NULL, atendimento, (void*) r);

        current++;
      } else {
        r->recusas++;

        write(FD_REJEITADOS, r, sizeof(Pedido));

        if (r->genero == 'M') REJEITADO_M++;
        else REJEITADO_F++;

        if (r->recusas != 3) PEDIDOS_EM_ESPERA++;

        //Logs the request.
        gettimeofday(&stop, NULL); //Stops counting time.
        double elapsed = (stop.tv_sec - inicio.tv_sec)*1000.0f + (stop.tv_usec - inicio.tv_usec) / 1000.0f;
        LOG_FILE = fopen(LOG_MSG_PATH, "a"); //Opens log file.
        fprintf(LOG_FILE, "%9.2f - %4d - %15lu - %2d - %c - %5d - %8s\n", elapsed, getpid(), pthread_self(), r->id, r->genero, r->duracao, tip[1]);
        fclose(LOG_FILE); //Closes log file.
      }
    }
  }
  close(FD_REJEITADOS);

  for(int i = 0; tid[i] != 0; i++){
    pthread_join(tid[i], NULL);
  }
  return NULL;
}

int main(int argc, char* argv[]){

  gettimeofday(&inicio, NULL);

  if (argc != 2){
    printf("Argumentos invalidos! USO: sauna <n. max utentes>\n");
    exit(-1);
  }

  capacidade = atoi(argv[1]);
  vagas = capacidade;
  GENERO_ATUAL = 'V'; //sauna "V"azia

  if (mkfifo(FIFO_REJEITADOS, S_IRUSR | S_IWUSR) != 0 && errno != EEXIST){
    perror("Erro ao criar o fifo de rejeitados");
    exit(EXIT_FAILURE);
  }

  while ((FD_REJEITADOS = open(FIFO_REJEITADOS, O_WRONLY)) == -1){
    if (errno == ENOENT || errno == ENXIO){
      printf("Esperando...\n");
      sleep(1);
    } else {
      perror("Erro ao abrir o fifo de rejeitados");
      exit(EXIT_FAILURE);
    }
  }

  criarFicheiroRegisto();

  pthread_t tid;
  pthread_create(&tid, NULL, handlerPedidos, NULL);
  pthread_join(tid, NULL);

  imprimirEstatisticas();

  pthread_exit(NULL);
}
