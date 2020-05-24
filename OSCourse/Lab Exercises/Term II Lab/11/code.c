#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(void)
{
    int pid;
    pid = fork();

    if(pid != 0) // we are in the parent process
    {
        sleep(2); // so that the first level of instructions get executed in the child process first
        printf("Sending the first SIGINT signal to child!\n");
        kill(pid, SIGINT);
        sleep(2);
        printf("Sending the second SIGINT signal to child!\n");
        kill(pid, SIGINT);

    }
    else
    {
        // we are in the child process
        struct sigaction* myaction = (struct sigaction*) calloc(1, sizeof(struct sigaction));
        myaction->sa_handler = SIG_IGN;
        sigaction(SIGINT, myaction, NULL); // configuring the child process to ignore the SIGINT signal
        printf("Changed the SIGINT's behavior to be ignored!\n");
        int i = 0;
        while(1)
        {
            sleep(2);
            printf("The child process is alive!\n");
            i++;
            if(i == 1)
            {
                myaction->sa_handler = SIG_DFL;
                sigaction(SIGINT, myaction, NULL);
                printf("Changed the SIGINT's behavior to default!\n");
            }          
        }
    }
    return 0;
}