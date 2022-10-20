#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#define MAX 1024

int rec = 0, reset = 0;


void terminate(){
    printf("3230shell: Terminated");
    exit(0);
}

char* shell_prompt(){
    static  char shell_cmd[MAX] ={0};
    printf("$$ 3230shell ##");
    fgets(shell_cmd,MAX,stdin);
    if (reset == 1){
        reset = 0;
        printf("\n");
        return shell_prompt();
    }

    if(strlen(shell_cmd) == 1){
        return shell_prompt();
    }
    else{
        shell_cmd[strlen(shell_cmd)-1] = 0;
        return shell_cmd;
    }
}

void sig_handler(int signum){

    if(signum == SIGUSR1){
        rec = 1;
    }
    if(signum == SIGINT){
        reset = 1;
    }

}


int main(void){
    struct rusage timeX;
    while(1){
        pid_t pid;
        int is_error = 0;
        char *input_cmd[MAX] = {NULL,};
        char *in_put[MAX] ={NULL,};
        //char *inputs;
        int i = 0, pipe = 0;


        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));

        sigaction(SIGINT, NULL, &sa);
        sigaction(SIGUSR1, NULL, &sa);

        sa.sa_handler = sig_handler;

        sigaction(SIGUSR1, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);

        char *inputs = shell_prompt(); // prompt display




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
            if(strcmp(in_put[j],"|") == 0)
                pipe++;
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
            while(!rec);

            if(execvp(in_put[0],in_put) == -1){
                printf("3230shell: \'%s\': %s\n",in_put[0],strerror(errno));
                exit(-1);
            }

        } else{
            kill(pid, SIGUSR1);
            waitpid(pid,NULL,0);

            //
            getrusage(RUSAGE_CHILDREN,&timeX);
            printf("(PID)%d   (CMD)%s", pid,in_put[0]);
            printf("(user)%ld.%03ld s", timeX.ru_utime.tv_sec,timeX.ru_utime.tv_usec/1000);
            printf("(sys)%ld.%03ld s",timeX.ru_stime.tv_sec, timeX.ru_stime.tv_usec/1000);
            //
            continue;
        }



    }
}





