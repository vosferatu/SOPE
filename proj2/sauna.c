#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define EXIT_ERROR -1

int capacity, vacant_places;

int main(int argc, char* argv[])
{
    if(argc != 2){
        printf("Wrong number of arguments! USAGE: sauna <n. max users>\n\n");
        exit(EXIT_ERROR);
    }

    capacity = atoi(argv[1]);
    vacant_places = capacity;

    return 0;
}
