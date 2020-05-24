#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    int pipefd[2];
    if(pipe(pipefd) == 0)
        printf("Pipe created successfully!\n");

    int pid;
    pid = fork();

    if(pid != 0)
    {
        // we are in the child process
        close(pipefd[0]); // closing the read end of the pipe
        dup2(pipefd[1],1); // something which would otherwise be written on the terminal gets written to pipefd[1] fd instead
        char *argv[] = {"/bin/ls", "-l", 0};
        execve("/bin/ls", argv, NULL);
        printf("execve failed!\n");
    }
    else
    {
        close(pipefd[1]); // closing the write end of the file
        sleep(1);
        dup2(pipefd[0], 0);
        char* argv[] = {"/usr/bin/wc", 0};
        execve("/usr/bin/wc", argv, NULL);
        printf("execve failed!\n");
    }
    return 0;
}