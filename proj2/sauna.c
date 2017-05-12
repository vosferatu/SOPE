#include <stdlib.h>
#include <stdio.h>

#define EXIT_ERROR -1

int main(int argc, char* argv[])
{
    if(argc != 2){
        printf("Wrong number of arguments! USAGE: sauna <n. max users>\n\n");
        exit(EXIT_ERROR);
    }
}
