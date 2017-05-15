#define MAX_FIFONAME 16
#define FIFO_MODE 0777

#define REJEITADOS "/tmp/rejeitados"
#define ENTRADA "/tmp/entrada"

typedef struct {
    int id;
    int duration;
    char gender;
    int refused;
} pedido_t;