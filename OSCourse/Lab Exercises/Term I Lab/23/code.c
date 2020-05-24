#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int ppid = getpid();
	fork();
	printf("Process id of the parent process: %d\n",ppid);
	int cpid = getpid();
	while(1){
	if(ppid == cpid)
	{
		continue;
	}
	else
	{
		printf("%d becomes a zombie process!\n", cpid);
		break;
	}
	}
	return 0;

}
