#include <stdio.h>
#include <unistd.h>
#include <signal.h> 
#include <stdlib.h>

void general_purpose_signal_catcher(int signum)
{
    printf("The custom signal handler got invoked!\n");
    if(signum == SIGSEGV)
        printf("Caught SIGSEGV!\n");
    else if(signum == SIGINT)
        printf("Caught SIGINT!\n");
    else if(signum == SIGFPE)
        printf("Caught SIGFPE!\n");
    else
        printf("Did not catch either of the three signals as defined above :(\n");
    return;
}

int main()
{
    int pid;
    pid = fork();

    if(pid != 0) // we are in the parent process
    {
        sleep(2);
        printf("Sending SIGSEGV signal to child!\n");
        kill(pid, SIGSEGV);
        sleep(1);
        printf("Sending SIGINT signal to child!\n");
        kill(pid, SIGINT);
        sleep(1);
        printf("Sending SIGFPE signal to child!\n");
        kill(pid, SIGFPE);
        sleep(1);
        kill(pid, SIGKILL);
    }
    else
    {
        struct sigaction* myaction = (struct sigaction*)calloc(1, sizeof(struct sigaction));
        myaction->sa_handler = general_purpose_signal_catcher;
        myaction->sa_flags = 0;
        int succ = sigaction(SIGSEGV, myaction, NULL);
        succ = succ + sigaction(SIGINT, myaction, NULL);
        succ = succ + sigaction(SIGFPE, myaction, NULL);
        if(succ == 0)
            printf("All signal actions changed successfully!\n");
        while(1){}
    }
    return 0;
}