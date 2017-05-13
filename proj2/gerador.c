#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

	typedef struct pedido{
		char gender;
		int d;
	}Pedido;

void gerador(int nPedidos, int maxUtilizacao){
	int pedidos[nPedidos];
	int counter = 0;
	char gender = '';
	int d=0;
	while(counter < nPedidos){
		int r = rand() % 3;
		if(r==1)
			gender = 'M';
		else gender = 'F';

		d = rand() % (maxUtilizacao+1);
		Pedido p;
		p.gender = gender;
		p.d = d;
	}
}


