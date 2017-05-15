DEPS = pedido.h

all: gerador.c sauna.c
	gcc -o balcao sauna.c -Wall -lpthread -lrt
	gcc -o gerador gerador.c -Wall -lpthread -lrt