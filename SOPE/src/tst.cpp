#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define MAX_NUMBER_LINE 500

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);
	if (argc != 3)
	{
		printf("Wrong number of arguments\n");
		exit(EXIT_FAILURE);
	}

	int nPedidos = atoi(argv[1]);
	int maxUtilizacao = atoi(argv[2]);


	int i = 0;
	printf("\nnPedidos: %d", nPedidos);
	while (i < nPedidos)
	{
		pid_t pidF = fork();
		if (pidF < 0)
		{
			perror("fork()");
			exit(EXIT_FAILURE);
		}
		else if (pidF == 0)
		{
			char fifoName[MAX_NUMBER_LINE];
			strcpy(fifoName, "fc_");
			char pid[MAX_NUMBER_LINE];
			sprintf(pid, "%d", getpid());
			strcat(fifoName, pid);
		}
	}
}

void gerador(int nPedidos, int maxUtilizacao){
	int counter = 0;
	while(counter < nPedidos){
		counter++;

	}
}
