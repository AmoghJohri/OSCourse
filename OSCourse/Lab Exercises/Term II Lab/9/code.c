#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int pid;
    pid = fork();

    if(pid != 0)
    {
        // we are in the parent process
        sleep(1); // so that signal(SIGINT, SIG_IGN) gets executed before this 
        printf("Sending the first SIGINT signal!\n"); 
        kill(pid, SIGINT); // a signal is sent to terminate the child process
        sleep(2); // sleeps for 2 seconds to observe the bhavior of the child process
        printf("Sending the seconds SIGINT signal!\n"); 
        kill(pid, SIGINT); // sends another signal to terminate the child process
        
    }
    else
    {
        int i = 0;
        signal(SIGINT, SIG_IGN); // this gets executed before the parent process
        while(1){
            sleep(1); // this is just to ensure that the terminal does not get cluttered with a lot of print statements
            printf("The child process is alive!\n"); // to display the current status of the child process
            i = i + 1;
            if(i == 1)
                signal(SIGINT, SIG_DFL); // restoring the signal's behavior to default, this occurs befor the second signal is sent from the parent process
        }
    }
    return 0;
}