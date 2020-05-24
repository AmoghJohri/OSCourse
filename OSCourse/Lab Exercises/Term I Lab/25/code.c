#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<signal.h>

int main()
{
	int ppid = getpid();
	int c1 = 0;
	int c2 = 0;
	int c3 = 0;
    int* wstatus = (int*)calloc(1, sizeof(int));
	printf("Process id of the parent process: %d\n", ppid);
	c1 = c1 + fork();
    int pid = getpid();
    if(ppid == pid)
    {
        c2 = c2 + fork();
        pid = getpid();
        if(ppid == pid)
        {
            c3 = c3 + fork();
        }
    }
    pid = getpid();
    if(ppid == pid)
    {

        printf("waitpid returns: %d\n",waitpid(c2,wstatus,0));
        printf("Child 1: %d\n",c1);
        printf("Child 2: %d\n",c2);
        printf("Child 3: %d\n",c3);
    }

    return 0;
}