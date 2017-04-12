#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
Invocação do find:

> find (starting directory) (matching criteria and actions)
*/

struct Flags{
    char* name;
    char* type;
    int mode;
    int print;
    int delete;
} flags;

void printUsage(){
    printf("usage:\n");
    printf("  sfind (starting directory) (matching criteria and actions)\n");
    printf("\t-name string (string is the exact name of the file)\n");
    printf("\t-type c (c is a character that can be f -normal file-, d -directory-, l -link symbolic)\n");
    printf("\t-perm mode (mode is an octal value corresponding to the access permissions to a file)\n");
    printf("\t-print (displays on screen the name of the files found)\n");
    printf("\t-delete (delete the file)\n");
    printf("\t-exec command (execute command - note: the files found are represented by {} in command)\n\n");
}

int procArgs(int argc, char *argv[]){

    if (argc < 3){
        return -1;
    }
    else for(size_t i = 2; i < argc; i++) {

        if(strcmp(argv[i],"-name") == 0) {
            flags.name = argv[i+1];
            i++;
        }
        else if(strcmp(argv[i],"-type") == 0) {
            flags.type = argv[i+1];
            i++;
        }
        else if(strcmp(argv[i],"-perm") == 0) {
            flags.mode = strtoul(argv[i+1],NULL,8);
            i++;
        }
        else if(strcmp(argv[i],"-print") == 0){
            flags.print = 1;
        }
        else if(strcmp(argv[i],"-delete") == 0){
            flags.delete = 1;
        }
        else if(strcmp(argv[i],"-exec") == 0){
            //mudar flag do command = argv[i+1]
            i++;
        }
        else {
            // se chegar até aqui é porque o argumento é invalido
            fprintf(stderr, "sfind: %s: unknown primary or operator\n", argv[i]);
            return 1;
        }

    }

    return 0;
}

int main(int argc, char *argv[]){

    char* buff[256];
    char* path;
    path = getcwd(NULL,sizeof(buff));

    printf("\ninitial path: %s\n\n", path);
    int valide = procArgs(argc, argv);
    switch (valide) {
        case -1: //Invalide args
        {
            printUsage();
            return -1;
        }

        case 0:
        {
            //Run the program
            printf("Sucesso ao ler os %d argumentos\n", argc);

            return 0;
        }

        case 1: //error in args
        {
            return 1;
        }

    }

}
