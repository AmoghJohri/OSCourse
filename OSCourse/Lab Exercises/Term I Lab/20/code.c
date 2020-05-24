#include<stdio.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
	int pid = getpid();
	printf("Priority of the process is: %d\n", getpriority(PRIO_PROCESS, pid));
	nice(1);
	pid = getpid();
	printf("Priority of the process is: %d\n", getpriority(PRIO_PROCESS, pid));
	
	return 0;
}

