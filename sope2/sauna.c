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

int main(int argc, char *argv[]){
    
    //Check input
    if(argc != 2){
        fprintf(stderr, "usage: sauna <n. lugares>\n");
        return 1;
    }
    
}