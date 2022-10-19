#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#define MAX 1024
#define CMD_MAX 5

int rec = 0, reset = 0;


void terminate(){
    printf("3230shell: Terminated");
    exit(0);
}

char * shell_prompt(char cmd[]){


    printf("$$ 3230shell ## ");
    fgets(cmd,MAX,stdin);

    //printf("shell_cmd[MAX]is %s and length is %lu",cmd,strlen(cmd));
    if(strlen(cmd) == 1 || reset ==1){
        cmd = "NULL";
        return cmd;
    }
    else{
        cmd[strlen(cmd)-1] = 0;
        return cmd;
    }
}

void sig_handler1(int signum){

    if(signum == SIGUSR1){
        rec = 1;
    }
    if(signum == SIGINT){
        reset = 1;
    }

}



int main(void){
    int status;
    pid_t pid;
    int pfd1[2];
    int pfd2[2];

    struct sigaction sa;

    sigaction(SIGINT, NULL, &sa);
    sigaction(SIGUSR1, NULL, &sa);

    sa.sa_handler = sig_handler1;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    while(1){
        int status; // status checking by parent process

        int is_error = 0;

        char *input_cmd[MAX] = {NULL,};
        char *in_put[MAX] ={NULL,};
        char shell_cmd[MAX] ={0};

        int i = 0 , pipe_cnt =0, cmd_cnt = 0;




        char *inputs = shell_prompt(shell_cmd);


        if(strcmp(inputs,"NULL") == 0){
            if(reset == 1){
                printf("\n");
                reset = 0;
            }
            continue;
        }

        // 1. command parse 한다.

        input_cmd[0] = strtok(inputs," ");
        printf("%s\n",input_cmd[0]);

        while(input_cmd[i] != NULL){
            i++;
            input_cmd[i] = strtok(NULL," ");
            if(input_cmd[i] == NULL){
                break;
            }
            //printf("%s\n",input_cmd[i]);
        }

        //EXIT
        if (strcmp(input_cmd[0],"exit") == 0){
            if (i > 1){
                printf("3230shell: \"exit\" with other arguments!!!!\n");
                continue;
            }
            else{
                printf("3230shell: Terminated\n");
                exit(0);
            }
        }


        // 2. pipe 오류를 확인한다.


        if (strcmp(input_cmd[0],"|") == 0){
            printf("3230shell: | cannot be placed at the front\n");
            is_error = 1;
        }

        if(is_error == 0){
            for( int j = 1; j< i; j++){
                if((strcmp(input_cmd[j],"|") ==0 && strcmp(input_cmd[j -1],"|") ==0)||strcmp(input_cmd[j],"||") ==0){
                    printf("3230shell: should not have two consecutive | without in-between command\n");
                    is_error = 1;
                    break;
                }
                if (j == i -1  && strcmp(input_cmd[j],"|") == 0 || j == i -1  && strcmp(input_cmd[j],"||") ==0){
                    printf("3230shell:  | cannot be placed at the back\n");
                    is_error = 1;
                    break;
                }
        }
        }

        if (is_error == 1)
            continue;


        // 3. pipe 확인한 이후에 명령어를 별도로 뽑아낸다.

        int j = 0;

        while(j < i){
            in_put[j] = input_cmd[j];
            if(strcmp(in_put[j],"|") == 0)
                pipe_cnt++;
            j++;
        }

        cmd_cnt = pipe_cnt + 1;

        printf("pipe num is %d\n",pipe_cnt);
        printf("command num is %d\n",cmd_cnt);

        if (cmd_cnt > CMD_MAX){
            printf("3230shell: Command cann be executed at most 5\n");
            continue;
        }




        //printf("j is %d\n\n",j);


        // 명령어 받아오기
        int pos =0;

        if(pipe_cnt > 1){
            pipe(pfd1);
            pipe(pfd2);
        }

        for (int cmd_loop = 0; cmd_loop < cmd_cnt; cmd_loop++) {
            printf("check\n\n\n\"");
            char *ind_cmd[30] = {NULL,};
            int index = 0;// command 선

            while (pos < j && strcmp(in_put[pos], "|") != 0 ) {
                ind_cmd[index] = in_put[pos];
                printf("%s\n", ind_cmd[index]);
                index++;
                pos++;

            }
            pos++;
            // command execution


            pid = fork();

            if (pid <0){
                printf("fork: error no %s\n", strerror(errno));
                exit(-1);
            } else if (pid == 0) {
                while(!rec);

                //signal reset
                sa.sa_handler = SIG_DFL;
                sigaction(SIGINT, &sa, NULL);
                sigaction(SIGUSR1, &sa, NULL);
                //
                if(cmd_loop == 0 && pipe_cnt >1){ // first pipe
                    close(pfd2[0]);
                    close(pfd2[1]);
                    close(pfd1[0]); //set pipe 1 write end to stdout
                    dup2(pfd1[1], 1);

                }
                if( 0 < cmd_loop && cmd_loop < pipe_cnt && pipe_cnt > 1){ // piping in between the commands.
                    close(pfd1[1]); //close pipe1 write end
                    close(pfd2[0]); //close pipe2 read end
                    dup2(pfd1[0], 0); //set pipe1 read end to stdin
                    dup2(pfd2[1], 1); //set pipe2 write end to stdout
                }
                if(cmd_loop == pipe_cnt && pipe_cnt > 1){
                    close(pfd1[0]); //close pipe1
                    close(pfd1[1]);
                    close(pfd2[1]); //close pipe2 write end
                    dup2(pfd2[0], 0); //set pipe2 read end to stdin
                }


                if(execvp(ind_cmd[0],ind_cmd) == -1){
                    printf("3230shell: \'%s\': %s\n",in_put[0],strerror(errno));
                    exit(-1);
                }
            } else{
                kill(pid, SIGUSR1);

                sa.sa_handler = SIG_IGN;
                sigaction(SIGINT, &sa, NULL);

                wait(&status);
                sa.sa_handler = sig_handler1;
                sigaction(SIGINT, &sa, NULL);
                break;
            }
        }



        //for (int cmd_loop = 0; cmd_loop < cmd_cnt; cmd_loop++){














        // 4. 돌린다.




        /*
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
                pipe_cnt++;
            j++;
        }

        in_put[j] = input_cmd[j];

        int


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
        //pipe 하자~~~


        for (int loop =0; loop <= pipe_cnt; loop++){ //since loop must have to run once.

            if(pipe_cnt > 1){
                pipe(pfd1);
                pipe(pfd2);
            }
            pid = fork();

            if (pid <0){
                printf("fork: error no %s\n", strerror(errno));
                exit(-1);
            } else if (pid == 0) {
                while(!rec);

                //signal reset
                sa.sa_handler = SIG_DFL;
                sigaction(SIGINT, &sa, NULL);
                sigaction(SIGUSR1, &sa, NULL);
                //
                if(loop == 0 && pipe_cnt >1){ // first pipe
                    close(pfd2[0]);
                    close(pfd2[1]);
                    close(pfd1[0]); //set pipe 1 write end to stdout

                }
                if(loop != pipe_cnt && pipe_cnt > 1){ // piping in between the commands.
                    close(pfd1[1]); //close pipe1 write end
                    close(pfd2[0]); //close pipe2 read end
                    dup2(pfd1[0], 0); //set pipe1 read end to stdin
                    dup2(pfd2[1], 1); //set pipe2 write end to stdout
                }
                if(loop == pipe_cnt && pipe_cnt > 1){
                    close(pfd1[0]); //close pipe1
                    close(pfd1[1]);
                    close(pfd2[1]); //close pipe2 write end
                    dup2(pfd2[0], 0); //set pipe2 read end to stdin
                }


                if(execvp(in_put[0],in_put) == -1){
                    printf("3230shell: \'%s\': %s\n",in_put[0],strerror(errno));
                    exit(-1);
                }
            } else{
                kill(pid, SIGUSR1);

                sa.sa_handler = SIG_IGN;
                sigaction(SIGINT, &sa, NULL);

                wait(&status);

                sa.sa_handler = sig_handler1;
                sigaction(SIGINT, &sa, NULL);
                continue;
            }

        }

       */
    }
    return 0;
}





