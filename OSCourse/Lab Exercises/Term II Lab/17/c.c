#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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
        close(1);
        fcntl(pipefd[1], F_DUPFD, 0); // something which would otherwise be written on the terminal gets written to pipefd[1] fd instead
        char *argv[] = {"/bin/ls", "-l", 0};
        execve("/bin/ls", argv, NULL);
        printf("execve failed!\n");
    }
    else
    {
        close(pipefd[1]); // closing the write end of the file
        sleep(1);
        close(0);
        fcntl(pipefd[0], F_DUPFD, 0);
        char* argv[] = {"/usr/bin/wc", 0};
        execve("/usr/bin/wc", argv, NULL);
        printf("execve failed!\n");
    }
    return 0;
}