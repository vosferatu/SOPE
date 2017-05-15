#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include "pedido.h"

FILE* record;
pthread_mutex_t car;
pthread_mutex_t logg;

int M_PEDIDOS = 0;
int F_PEDIDOS = 0;

void* request(void* arg){
    
    clock_t time;
    time = clock();//gets time since program start
    printf("time: %lu\n", time);    


    pedido_t* pedido = (pedido_t*) arg;
    int fd;
    
    sem_t *semaphore = sem_open("sem", O_CREAT ,0660,1);
    sem_wait(semaphore);

    fd = open(ENTRADA, O_WRONLY | O_NONBLOCK);
    
    if(fd == -1){
        printf("request: Unable to open fifo %s\n", ENTRADA);
        pthread_exit(NULL);
        sem_post(semaphore);
    }
    
    pthread_mutex_lock(&car);
    write(fd, &(pedido->id), sizeof(int));
    write(fd, &(pedido->duration), sizeof(int));
    write(fd, &(pedido->gender), sizeof(char));
    write(fd, &(pedido->refused), sizeof(int));
    pthread_mutex_unlock(&car);
    sem_post(semaphore);
    sem_close(semaphore);
    
    free(pedido);
    pthread_exit(NULL);
    
    
}

void new_request(int t_max){
    
    static int ID = 1;
    pedido_t* pedido = (pedido_t*) malloc(sizeof(pedido_t));
    
    pedido->id = ID; ID++;
    pedido->refused = 0;
    
    pedido->duration = (rand() % t_max) + 1;
    
    if((rand() % 2) == 0){
        pedido->gender = 'F';
        F_PEDIDOS++;
    }
    else {
        pedido->gender = 'M';
        M_PEDIDOS++;
    }
    
    pthread_t td;
    pthread_create(&td, NULL, request, (void *) pedido);
    pthread_detach(td);
    
}

void generator_init(int pedidos, int t_max){
    
    while(pedidos > 0){
            new_request(t_max);
    }
    
}

int main(int argc, char *argv[]){
    
    //Check input
    if(argc != 3){
        fprintf(stderr, "usage: gerador <n. pedidos> <t max. utilizacao>\n");
        return 1;
    }
    
    time_t tempo;
    srand((unsigned) time(&tempo));
    
    int pedidos = atoi(argv[1]);
    int t_max = atoi(argv[1]);
    
    if(pedidos < 0) pedidos = -pedidos;
    if(pedidos == 0) pedidos = (rand() % 2000) + 500;
    
    if (t_max < 0) t_max = - t_max;
    if(t_max == 0) t_max = (rand() % 1000) + 200;
    
    int pid;
    pid = getpid();
    char balPathname [20];
    sprintf (balPathname, "/tmp/bal.%d", pid);
    
    record = fopen(balPathname, "w");
    
    if(record == NULL) printf(". SAUNA: Error opening balFile\n");
    
    pthread_mutex_init(&car, NULL);
    pthread_mutex_init(&logg, NULL);
    
    generator_init(pedidos, t_max);
    
    pthread_mutex_destroy(&car);
    pthread_mutex_destroy(&logg);
    
    if(record != NULL)
        fclose(record);
    
    pthread_exit(NULL);
    
    return 0;
    
    
}
