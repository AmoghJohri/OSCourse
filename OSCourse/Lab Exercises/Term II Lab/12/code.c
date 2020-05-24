#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    int pid;
    pid = fork();

    if(pid != 0)
    {
        while(1)
        {
            sleep(1);
            printf("Parent process is alive!\n");
        }
    }
    else
    {
        while(1)
        {
            printf("Child process is alive!\n");
            sleep(1);
            if(kill(getppid(), SIGKILL) == 0)
            {
                while(1)
                {
                    sleep(1);
                    printf("This is now an orphan process!\n");
                }
            }
        }
    }
}