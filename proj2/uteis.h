
//Fifos
char* FIFO_ENTRADAS = "/tmp/entrada";
char* FIFO_REJEITADOS = "/tmp/rejeitados";

//File descriptors
int FD_REJEITADOS;
int FD_ENTRADAS;


typedef struct {
    int id; //numero do pedido
    char genero;
    int duracao;
    int recusas;
} Pedido;
