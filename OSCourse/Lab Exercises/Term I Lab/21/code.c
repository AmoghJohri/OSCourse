#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
	int ppid = getpid();
	printf("Parent pid: %d\n",ppid);
	fork();
	int pid = getpid();
	if(pid != ppid)
		printf("Child pid: %d\n", pid);
	return 0;
}
