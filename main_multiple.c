#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <sys/resource.h>

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

    //pid_t pid;


    struct sigaction sa;

    sigaction(SIGINT, NULL, &sa);
    sigaction(SIGUSR1, NULL, &sa);

    sa.sa_handler = sig_handler1;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    while(1){
        //int status; // status checking by parent process

        pid_t pid, wpid;
        int is_error = 0;
        int is_timeX =0;
        char *input_cmd[MAX] = {NULL,};
        char *in_put[MAX] ={NULL,};
        char shell_cmd[MAX] ={0};

        int i = 0 , pipe_cnt =0;


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

        if (strcmp(input_cmd[0],"timeX") == 0){
            if (i == 1){
                printf("3230shell: \"timeX\" cannot be a standalone command\n");
                continue;
            }
            else{
                is_timeX = 1; // set to get the variables.
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

        int input_idx = 0;
        if(is_timeX == 1){
            input_idx = 1;
        }

        while(input_idx < i){
            in_put[input_idx] = input_cmd[input_idx];
            if(strcmp(in_put[input_idx],"|") == 0)
                pipe_cnt++;
            input_idx++;
        }


        int cmd_cnt = pipe_cnt + 1;

        // TIME_INDEX
        struct rusage timeX[cmd_cnt];
        int time_pid[5] ={0};
        char *time_cmd[5];
        int time_index = 0;


        if (cmd_cnt > CMD_MAX){
            printf("3230shell: Command cann be executed at most 5\n");
            continue;
        }


        //printf("j is %d\n\n",j);



        int fds[5][2];
        for(int init = 0;init < pipe_cnt;init++){ // if pipe_cnt = 4 --> only can use fds 0,1,2,3 -->fds[4]
            pipe(fds[init]);
        }



        int pos =0;

        for (int cmd_loop = 0; cmd_loop < cmd_cnt; cmd_loop++) {  // condition 1) when command is 1 (O) 2) when command is 2 3) when command is more than 2
            char *ind_cmd[30] = {NULL,};
            int index = 0;

            while (pos < input_idx && strcmp(in_put[pos], "|") != 0 ) {
                ind_cmd[index] = in_put[pos];
                index++;
                pos++;
            }
            if(is_timeX ==1)
                time_cmd[cmd_loop]= ind_cmd[0];


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


                if(cmd_loop == 0 && pipe_cnt >=1){ // first pipe e.g. if pipe total 4 and first count is like fds[3] fds[2] fds[1]

                    for(int i = pipe_cnt - 1 ; i> cmd_loop;i--){ // when commands are 2 -> fds[0] is made
                        close(fds[i][0]);
                        close(fds[i][1]);
                    }
                    dup2(fds[cmd_loop][1], 1);//set pipe 1 write end to stdout
                    close(fds[cmd_loop][0]); // close stdin

                }
                if( 0 < cmd_loop && cmd_loop < pipe_cnt  && pipe_cnt >= 1){ // if current is 3rd commdand cmd_loop =2
                    for(int i = pipe_cnt - 1  ; i> cmd_loop;i--){ // when commands are 5 and we are concerning 3rd cmd cmd_loop is 2 and pipe is 4
                        close(fds[i][0]);                         // then close fds[0] and fds[3]
                        close(fds[i][1]);                         // fds[3] is closed
                    }
                    for(int i = 0 ; i < cmd_loop - 1 ;i++){ //fds[0]
                        close(fds[i][0]);
                        close(fds[i][1]);
                    }
                    // fds[1] fds[2] left
                    dup2(fds[cmd_loop-1][0],0);
                    dup2(fds[cmd_loop-0][1],1);
                    close(fds[cmd_loop-1][1]); // fds[1]
                    close(fds[cmd_loop][0]);

                }
                if(cmd_loop == pipe_cnt && pipe_cnt >= 1){ // when commands are 5 and we are concerning 3rd cmd cmd_loop is 2 and pipe is 4

                    for(int i = 0 ; i < cmd_loop -1;i++){           // consider last command 5th then cmd_loop is 4
                        close(fds[i][0]);
                        close(fds[i][1]);
                    }
                    dup2(fds[cmd_loop-1][0],0);
                    close(fds[cmd_loop-1][1]);

                }

                if(execvp(ind_cmd[0],ind_cmd) == -1){
                    printf("3230shell: \'%s\': %s",in_put[0],strerror(errno));
                    exit(-1);
                }

            } else { // when process is a parent process

                kill(pid, SIGUSR1);
                time_pid[cmd_loop] = pid;
                sa.sa_handler = SIG_IGN;
                sigaction(SIGINT, &sa, NULL);
                if (cmd_loop + 1 < cmd_cnt) {
                    continue;
                }

                for (int i = 0; i < pipe_cnt; i++) { //close pipes for the parent

                    close(fds[i][0]);
                    close(fds[i][1]);
                }
                while (wpid = wait(&status) > 0) {
                    getrusage(RUSAGE_CHILDREN, &timeX[time_index]);
                    time_index++;

                }

            }
        }

        for (int i = 0; i< time_index; i++){
            if(is_timeX ==1){
                printf("(PID)%d   (CMD)%s   ", time_pid[i],time_cmd[i]);
                printf("(user)%ld.%06ld s   ", timeX[i].ru_utime.tv_sec,timeX[i].ru_utime.tv_usec);
                printf("(sys)%ld.%06ld s\n",timeX[i].ru_stime.tv_sec, timeX[i].ru_stime.tv_usec);
            }

        }

    }
    return 0;
}













