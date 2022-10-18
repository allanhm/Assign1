

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#define MAX 1024

int rec = 0;

void terminate(){
    printf("3230shell: Terminated");
    exit(0);
}

char* shell_prompt(){
    static  char shell_cmd[MAX] ={0};
    printf("$$ 3230shell ##");
    fgets(shell_cmd,MAX,stdin);

    if(strlen(shell_cmd) == 1){
        return shell_prompt();
    }
    else{
        shell_cmd[strlen(shell_cmd)-1] = 0;
        return shell_cmd;
    }
}

void sig_handler(){

}


int main(void){


    pid_t pid;

    while(1){
        int is_error = 0;
        char *input_cmd[MAX] = {NULL,};
        char *in_put[MAX] ={NULL,};
        char *inputs={NULL,};
        int i = 0;


        inputs = shell_prompt(); // prompt display





        input_cmd[0] = strtok(inputs," ");

        if (strcmp(input_cmd[0],"|") == 0){
            printf("Error! | cannot be placed at the front\n");
            is_error = 1;
        }

        while(input_cmd[i] != NULL && is_error == 0){
            i++;
            input_cmd[i] = strtok(NULL," ");

            if(input_cmd[i]== NULL){
                if(strcmp(input_cmd[i -1],"|") ==0){
                    printf("Error! | cannot be placed at the back\n");
                    is_error = 1;
                }
                break;
            }
            if((strcmp(input_cmd[i],"|") ==0 && strcmp(input_cmd[i -1],"|") ==0)||strcmp(input_cmd[i],"||") ==0){
                printf("3230shell: should not have two consecutive | without in-between command\n");
                is_error = 1;
                break;
            }
        }
        if (is_error == 1)
            continue;




        int j = 0;
        while(j < i){
            in_put[j] = input_cmd[j];
            printf("%s\n",in_put[j]);
            j++;
        }

        in_put[j] = input_cmd[j];


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


        */
    }
}





