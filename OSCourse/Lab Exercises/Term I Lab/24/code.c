#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>

int main()
{
	int ppid = getpid();
	printf("Parent process id is: %d\n",ppid);
	fork();
	int cpid = getpid();
	if(cpid == ppid)
	{
		return 0;
	}
	printf("Child process id is: %d\n", cpid);
	printf("This becomes an orphan process!\n");
	while(1){}
}
	
