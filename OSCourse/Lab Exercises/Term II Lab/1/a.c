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

    // setting the timer
    if(setitimer(ITIMER_REAL, new_value, NULL))
    {
        printf("setitimer failed!\n");
        return -1;
    }

    sleep(1); // sleep for 1 second

    // checking the current set-timer
    struct itimerval* timer = (struct itimerval*)calloc(1, sizeof(struct itimerval));
    if(getitimer(ITIMER_REAL, timer) == -1)
    {
        printf("getitimerval failed!\n");
        return -1;
    }

    struct timeval buff;
    buff = timer->it_value;

    // checking the status of the current REAL timer
    long mu_sec_expired = 1000000 - buff.tv_usec;
    long sec_expired = 10 - buff.tv_sec;
    if(mu_sec_expired != 0)
        sec_expired --;
    printf("Time Expired : %ld seconds and %ld micro-seconds\n", sec_expired, mu_sec_expired);
    
    // the program however, exits before the timer can run down to zero
    return 0;
}