#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


int main()
{
    int pipefd[2];
    int msgsize = 16;
    if(pipe(pipefd) == 0)
        printf("Pipe created successfully!\n");

    int pid;
    pid = fork();
    
    if(pid != 0)
    {
        // we are in the parent process
        char* s = "Hello World!\n";
        printf("Sending the message from the parent process!\n");
        if(write(pipefd[1], s, msgsize) > 0)
            printf("Data sent successfully!\n");
        while(1){};

    }
    else
    {
        sleep(2); // sleeping for two seconds so data can be sent to the child process
        char* buffer = (char*)calloc(msgsize, sizeof(char));
        if(read(pipefd[0], buffer, msgsize) > 0)
        {
            printf("Data read successfully!\n");
            printf("Data: %s", buffer);
        }
        kill(getppid(), SIGKILL);
    }
    return 0;
}