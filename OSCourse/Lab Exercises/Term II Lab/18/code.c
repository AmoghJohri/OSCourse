#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int pipefd[2];
    if(pipe(pipefd) == 0)
        printf("First pipe created successfully!\n");
    int pid = fork();
    if(pid != 0)
    {
        // we are in the parent process
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        char* argv[] = {"/bin/ls", "-l", 0};
        execve("/bin/ls", argv, NULL);
    }
    else
    {
        // we are in the child process
        close(pipefd[1]);
        sleep(1);
        dup2(pipefd[0], 0);
        int pipefd_2[2];
        if(pipe(pipefd_2) == 0)
            printf("Second pipe created successfully!\n");
        int pid_2 = fork();
        if(pid_2 != 0)
        {
            close(pipefd_2[0]);
            dup2(pipefd_2[1], 1);
            char* argv_2[] = {"/bin/grep", "^d", 0};
            execve("/bin/grep", argv_2, NULL);
        }
        else
        {
            close(pipefd_2[1]);
            sleep(1);
            dup2(pipefd_2[0], 0);
            char* argv_2[] = {"/usr/bin/wc", 0};
            execve("/usr/bin/wc", argv_2, NULL);
            printf("execve failed!\n");
        }
    }
}