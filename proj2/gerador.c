#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#define BUF_LENGTH 256


int pedidosM=0, pedidosF=0, rejeitadosM=0, rejeitadosF=0, descartadosM=0, descartadosF=0;

	typedef struct pedido{
		int id;
		char gender;
		int d;
	}Pedido;

		char getGender(){
			char gender = '';
			int r = rand() % 2;
			if(r==0)
				gender = 'M';
			else gender = 'F';
			return gender;
		}

		int getRandomTime(){
			int d=0;
			d = rand() % maxUtilizacao + 1;
		}

		void escrevePedidoEmFicheiro(Pedido ped){
			FILE * dst= fopen("output", "w");
			if (dst != NULL) {
				fwrite(ped, sizeof(struct pedido), 1, dst);
				fclose(file);
			}
		}


void gerarPedidos(int nPedidos, int maxUtilizacao){
	int pedidos[nPedidos];
	int counter = 1;
	char buf[BUF_LENGTH];

	while(counter < nPedidos){
		pthread_mutex_lock(&mut); // lock thread

		Pedido *p = malloc(sizeof(struct pedido));
		p->id = counter;
		p->gender = getGender();
		p->d = getRandomTime();

		escrevePedidoEmFicheiro(p);

		counter++;

		if(p->gender=='M')
			pedidosM++;
		else pedidosF++;

		write(ENTRADA_FIFO_FD, &p, sizeof(pedido));
		pthread_mutex_unlock(&mut); // unlock thread
		sleep(2);
	}

	close(ENTRADA_FIFO_FD);
	return NULL;
}


	int main(int argc, char *argv[])
	{

		// get program start timing
		    clock_gettime(CLOCK_MONOTONIC, &startTimespec);

		// generate new random seed
		    srand(time(NULL));

		if (argc != 3)
		{
			printf("Numero invalido de argumentos! USO: gerador <nPedidos> <periodoMaximoUtilizacao>\n");
			exit(EXIT_FAILURE);
		}

		int nPedidos = atoi(argv[1]);
		int maxUtilizacao = atoi(argv[2]);

		//Criacao do FIFO de entrada
		 if (mkfifo("/tmp/entrada", S_IRUSR | S_IWUSR) != 0) {
			 if (errno==EEXIST)
				 printf("FIFO '/tmp/entrada' já existe\n");
		        else
		        printf("Nao foi possivel criar o FIFO '/tmp/entrada'\n");
		        exit(EXIT_FAILURE);
		    }
		 else printf("FIFO '/tmp/entrada' criada com sucesso\n");

		 ENTRADA_FIFO_FD = open("/tmp/entrada", O_WRONLY | O_NONBLOCK);
}
