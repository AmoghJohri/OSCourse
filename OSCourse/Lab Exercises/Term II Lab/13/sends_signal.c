#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    printf("Enter the pid of the process: ");
    int p;
    scanf("%d",&p);
    printf("Sending SIGSTOP signal to the process!\n");
    if(kill(p, SIGSTOP) == 0)
        printf("Successfully sent the signal!\n");
    else
        printf("Failed to send the signal!\n");
    return 0;
}