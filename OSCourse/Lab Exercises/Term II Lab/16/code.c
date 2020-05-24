#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


int main()
{
    // we will be using two pipes to perform two way communication
    int pipefd1[2];
    int pipefd2[2];

    int msgsize = 15;

    if(pipe(pipefd1) + pipe(pipefd2) == 0)
        printf("Pipes created successfully!\n");

    
    int pid;
    pid = fork();

    if(pid != 0)
    {
        // we are in the parent process
        char* s = "Parent to child";
        if(write(pipefd1[1], s, msgsize) > 0)
            printf("Data sent from parent!\n");
        sleep(2);
        char* buff = (char*)calloc(msgsize, sizeof(char));
        if(read(pipefd2[0], buff, msgsize) > 0)
            printf("Data read in parent: %s\n", buff);
        sleep(1);

    }
    else
    {
        sleep(2);
        char* buff = (char*)calloc(msgsize, sizeof(char));
        if(read(pipefd1[0], buff, msgsize) > 0)
        {   
            printf("Data read in child: %s\n", buff);
        }
        char* s = "Child to parent";
        if(write(pipefd2[1], s, msgsize) > 0)
            printf("Data sent from child!\n");
        sleep(1);
    }
    return 0;
}