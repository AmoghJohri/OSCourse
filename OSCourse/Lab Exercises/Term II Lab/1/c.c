#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

int main()
{
    // setting a single-shot timer of 10 seconds and 10 micro-seconds
    struct timeval *interval = (struct timeval*)calloc(1, sizeof(struct timeval));
    interval->tv_sec = 0;
    interval->tv_usec = 0; 
    struct timeval *value = (struct timeval*)calloc(1, sizeof(struct timeval));
    value->tv_sec = 10;
    value->tv_usec = 10;

    // setting the itimerval struct
    struct itimerval *new_value = (struct itimerval*)calloc(1, sizeof(struct itimerval));
    new_value->it_interval = (*interval);
    new_value->it_value = (*value);

    // setting the itimerval struct
    struct itimerval *old_value = (struct itimerval*)calloc(1, sizeof(struct itimerval));

    // setting the timer
    if(setitimer(ITIMER_PROF, new_value, old_value))
    {
        printf("setitimer failed!\n");
        return -1;
    }
    

    printf("New Value: %ld seconds and %ld micro-seconds\n", new_value->it_value.tv_sec, new_value->it_value.tv_usec);
    printf("Old Value: %ld seconds and %ld micro-seconds\n", old_value->it_value.tv_sec, old_value->it_value.tv_usec);

    
    return 0;
}