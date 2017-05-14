#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define BUF_LENGTH 256

	typedef struct pedido{
		char gender;
		int d;
	}Pedido;

void gerador(int nPedidos, int maxUtilizacao){
	int pedidos[nPedidos];
	int counter = 0;
	char gender = '';
	int d=0;
	char buf[BUF_LENGTH];

	while(counter < nPedidos){
		int r = rand() % 3;
		if(r==1)
			gender = 'M';
		else gender = 'F';

		d = rand() % (maxUtilizacao+1);
		Pedido *p = malloc(sizeof(struct pedido));
		p->gender = gender;
		p->d = d;


		FILE * dst= fopen("output", "w");
		if (dst != NULL) {
		    fwrite(p, sizeof(struct pedido), 1, dst);
		    fclose(file);
		}
	}


	int main(int argc, char *argv[])
	{

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

