#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

void SIGSEGVHandler(int sig)
{
    printf("Caught SIGSEGV!\n");
    return;
}

void SIGINTHandler(int sig)
{
    printf("Caught SIGINT!\n");
    return;
}

void SIGFPEHandler(int sig)
{
    printf("Caught SIGFPE!\n");
    return;
}

void SIGALRMHandler(int sig)
{
    printf("Caught SIGALRM!\n");
    return;
}

void SIGVTALRMHandler(int sig)
{
    printf("Caught SIGVTALRM!\n");
    return;
}

void SIGPROFHandler(int sig)
{
    printf("Caught SIGPROF!\n");
    return;
}

int main() 
{ 
    int pid;
    pid = fork();

    if(pid == 0) // the child process goes into an infinite loop
    { 
        signal(SIGSEGV, SIGSEGVHandler); // if the child process receives this signal, it executes the custom signal handler
        signal(SIGINT, SIGINTHandler);
        signal(SIGFPE, SIGFPEHandler);
        signal(SIGALRM, SIGALRMHandler);
        signal(SIGVTALRM, SIGVTALRMHandler);
        signal(SIGPROF, SIGPROFHandler);
        
        alarm(1); // send SIGALRM signal to calling process
        sleep(2);

        struct itimerval* timer = (struct itimerval*)calloc(1, sizeof(struct itimerval));
        // setting a single-shot timer of 1 second
        struct timeval *interval = (struct timeval*)calloc(1, sizeof(struct timeval));
        interval->tv_sec = 0;
        interval->tv_usec = 0; 
        struct timeval *value = (struct timeval*)calloc(1, sizeof(struct timeval));
        value->tv_sec = 1;
        value->tv_usec = 0;
        timer->it_interval = *interval;
        timer->it_value = *value;

        setitimer(ITIMER_REAL, timer, NULL);
        sleep(2);
        setitimer(ITIMER_VIRTUAL, timer, NULL);
        sleep(2);
        setitimer(ITIMER_PROF, timer, NULL);
        sleep(2);

        
        while(1); 
    }
    else
    {  
        sleep(1); // sleeps for 1 second
        kill(pid, SIGSEGV);
        sleep(1); // sleeps for 1 second
        kill(pid, SIGINT);
        sleep(1); // sleeps for 1 second
        kill(pid, SIGFPE);
        sleep(10);
        kill(pid, SIGKILL);
    }
}