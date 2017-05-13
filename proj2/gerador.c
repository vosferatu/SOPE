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
 /*
		int fd2 = open("output", O_WRONLY | O_CREAT | O_EXCL, 0644);
		 if (fd2 == -1) {
		  perror(argv[2]);
		  close(fd1);
		  return 3;
		 }
		 */


		FILE * dst= fopen("output", "w");
		if (dst != NULL) {
		    fwrite(p, sizeof(struct pedido), 1, dst);
		    fclose(file);
		}
	}
}

