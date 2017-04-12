#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Invocação do find:

> find (starting directory) (matching criteria and actions)

*/


void printUsage(){
    printf("usage:\n");
    printf("  sfind (starting directory) (matching criteria and actions)\n");
    printf("\t-name string (string is the exact name of the file)\n");
    printf("\t-type c (c is a character that can be f -normal file-, d -directory-, l -link symbolic)\n");
    printf("\t-perm mode (mode is an octal value corresponding to the access permissions to a file)\n");
    printf("\t-print (displays on screen the name of the files found)\n");
    printf("\t-delete (delete the files)\n");
    printf("\t-exec command (execute command - note: the files found are represented by {} in command)\n\n");
    }

    int procArgs(int argc, char *argv[]){

        if (argc < 3){
            return -1;
        }
        else return 0;


    }

    int main(int argc, char *argv[]){

        int valide = procArgs(argc, argv);

        if (valide == -1){
            printUsage();
            return valide;
        }

    }
