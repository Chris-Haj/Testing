#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>
//#include <.h>
#define writeend 1
#define readend 0

void sendToPipe(char *cmd[], char *cmdout[]);

int main() {
    char *first[] = {"ls", "-l", NULL};
    char *second[] = {"sort", "-r", NULL};
    char *third[] = {"wc", NULL};
    char **commands[] = {first, second, third};
    int n = 3;
    int pipenum = n-1;
    int fd[2*pipenum];
//    for(int i=0;i<pipenum;i++)
//        pipe(&fd[i*2]);
    int status;
    pid_t pids[n];
    pids[0] = fork();

    for(int i=1;i<n;i++){
       if(pids[i-1] != 0)
           pids[i] = fork();
    }
    printf("i'm child number %d\n",getpid());
    for(int i=0;i<n;i++)
        wait(&status);

    return 0;
}

void sendToPipe(char *in[], char *out[]) {
    pid_t pid;
    int fd[2];

    pipe(fd);
    pid = fork();

    if (pid == 0) {
        dup2(fd[writeend], STDOUT_FILENO);
        close(fd[readend]);
        close(fd[writeend]);
        execvp(in[0], in);
        exit(1);
    } else {
        pid = fork();

        if (pid == 0) {
            dup2(fd[readend], STDIN_FILENO);
            close(fd[writeend]);
            close(fd[readend]);
            execvp(out[0], out);
            exit(1);
        } else {
            int status;
            close(fd[readend]);
            close(fd[writeend]);
            waitpid(pid, &status, 0);
        }
    }
}