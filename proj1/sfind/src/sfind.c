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

void sigint_handler(int signo){

  char terminate;

  kill(getppid(),SIGSTOP);

  printf("\n«Are you sure you want to terminate (Y/N)?»\n");
  scanf("%s",&terminate);

  terminate = (char)toupper(terminate);

  if(terminate == 'N'){
    kill(getppid(),SIGCONT);
  }
  else{
    kill(getppid(),SIGQUIT);
    kill(getpid(),SIGQUIT);
  }

}

void subscribe_SIGINT(){

  struct sigaction act;
  act.sa_handler = sigint_handler;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);

  sigaction(SIGINT,&act,NULL);

}

void searchDir(char* path){

    struct stat fileStatus;
    struct dirent *fileEntry;
    DIR* dp;
    pid_t pid;

    subscribe_SIGINT();

    if (stat(path,&fileStatus) != 0) printf("Stat error\n");

    if (S_ISDIR(fileStatus.st_mode)){

        if ((dp = opendir(path)) == NULL) printf("Error opendir\n");

        char newPath[256];

        while ((fileEntry = readdir(dp)) != NULL){

            //Ignore the current directory and previous directory folders.
            if (strcmp(fileEntry->d_name, ".") == 0 || strcmp(fileEntry->d_name, "..") == 0){
                continue;
            }
            //Update the current path.
            strcpy(newPath, path);
            strcat(newPath, "/");
            strcat(newPath, fileEntry->d_name);
            stat(newPath, &fileStatus);

            //Whether the read structure is a file.
            if (S_ISREG(fileStatus.st_mode)){
                if (flags.print == 1 && ((strcmp(flags.type,"f")==0) ||
                 strcmp(flags.name, fileEntry->d_name) == 0
                 || flags.mode == fileStatus.st_mode)){
                    printf("%s\n", newPath);
                }
            }
            //Whether the read structure is a directory.
            if (S_ISDIR(fileStatus.st_mode)){
                if (flags.print == 1 && (strcmp(flags.type,"d")==0)){
                    printf("%s\n", newPath);
                }
                switch (pid = fork()){
                    case -1:
                    //perror("Error fork()...\n");
                    break;

                    case 0: //child process
                    searchDir(newPath);
                    break;

                    default:
                    waitpid(pid,NULL,WNOHANG);
                    break;


                }
            }
        }
        closedir(dp);
    }
}

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

      flags.name = "";
      flags.type = "$";
      flags.mode = 022;
      flags.print = 0;
      flags.delete = 0;


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
            searchDir(path);

            return 0;
        }

        case 1: //error in args
        {
            return 1;
        }

    }

}
