#include <stdlib.h>
#include <unistd.h>

int pipes[2];

int main(){
    if (pipe(pipes) == -1)
        exit(1);

    if (fork() == 0){
        // output to pipes
        dup2(pipes[1], 1);

        // close reading end of pipe
        close(pipes[0]);

        // exec
        char *argv[] = {"ls", NULL};
        execv("/bin/ls", argv);
    }
    else{
        // input from pipes
        dup2(pipes[0], 0);

        // close writing end of pipe
        close(pipes[1]);

        char *argv[] = {"wc", "-l", NULL};
        execv("/usr/bin/wc", argv);
    }

}