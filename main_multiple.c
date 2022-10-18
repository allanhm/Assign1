

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
    static char shell_cmd[MAX] ={0};
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

    char *inputs;
    pid_t pid;
    char *input_cmd[MAX] = {NULL,};
    while(1){


        inputs = shell_prompt(); // prompt display
        printf("%s\n",inputs);



        char pipe_parse[strlen(shell_cmd)];
        /*
        strncpy(pipe_parse,shell_cmd,strlen(shell_cmd));


        int i =0;
        char* ptr;
        char* test =pipe_parse;
        int pipe_cnt =0;


        while((ptr=strsep(&test,"|")) != NULL) {

            i++;

            char check[MAX];
            int count = 0;
            strcpy(check,ptr);


            for (int j = 0; j <strlen(check); j++){
                if(isspace(check[j])!= 0){
                    count++;
                    }
                }
            printf("%s is a command\n\n\n",check);
            int len = (int)strlen(check);


            if ((len > 0 && len == count) || len == 0 && i > 1){
                printf("3230shell: should not have two consecutive | without in-between command\n");
                continue;
                printf("test\n");
            }

            if((len == 0 && i == 1 )|| ( len == 0 && test == NULL)){
                printf("ERROR!!!! | should not be place either at first or the last\n");
                continue;
                printf("test\n");
            }

            if(len > 0 && len != count){
                pipe_cnt++;
            }
            }
            printf("Normal Pipe is %d\n",pipe_cnt);


        input_cmd[0] = strtok(inputs," ");


        int i = 0;

        while(input_cmd[i] != NULL){
            i++;
            input_cmd[i] = strtok(NULL," ");
            if(input_cmd[i]== NULL){
                break;
            }
        }


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


        */
    }
}





