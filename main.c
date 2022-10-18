

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>
#define MAX 1024

void terminate(){
    printf("3230shell: Terminated");
    exit(0);
}

void shell_prompt(){
    printf("$$ 3230shell ##");
}

int main(void){

    char shell_cmd[MAX];
    char *inputs;
    pid_t pid;
    char* test;
    char *input_cmd[MAX] = {NULL,};
    while(1){


        shell_prompt(); // prompt display


        /* ----------------------------- */  // Command Input
        fgets(shell_cmd,MAX,stdin);

        shell_cmd[strlen(shell_cmd)-1] = 0;

        inputs = shell_cmd;
        printf("%s\n",inputs);
        /* ----------------------------- */


        /* ----------------------------- */  // Command Parsing//
        input_cmd[0] = strtok(inputs," ");

        if (strcmp(input_cmd[0],"|") == 0){
            printf("Error! | cannot be placed at the front\n");
            continue;
        }


        int i = 0;

        while(input_cmd[i] != NULL){
            i++;
            input_cmd[i] = strtok(NULL," ");
            if(input_cmd[i]== NULL){
                break;
            }
        }

        /* ----------------------------- */

        /* ----------------------------- */  // Command Input//
        char *in_put[i+1];

        int j = 0;
        while(j < i){
            in_put[j] = input_cmd[j];
            j++;
        }
        in_put[j] = input_cmd[i];


        if (strcmp(in_put[0],"exit") == 0){
            if (i > 1){
                printf("3230shell: \"exit\" with other arguments!!!!\n");
                continue;
            }
            else{
                printf("3230shell: Terminated\n");
                exit(0);
            }
        }

        pid = fork();
        if (pid <0){
            printf("fork: error no ");
            exit(-1);
        } else if (pid == 0) {

            if(execvp(in_put[0],in_put) == -1){
                printf("3230shell: \'%s\': %s\n",in_put[0],strerror(errno));
                exit(-1);
            }

        } else{
            waitpid(pid,NULL,0);
            continue;
        }



    }
}





