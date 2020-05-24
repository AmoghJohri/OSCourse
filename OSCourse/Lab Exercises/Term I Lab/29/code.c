#include<sched.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include <errno.h>

int main()
{

	printf("Run the binary execution file with sudo!\n");

	int ret = sched_getscheduler(0);
	printf("Current scheduling policy: %d\n",ret);

	struct sched_param* s = (struct sched_param*) calloc(1, sizeof(struct sched_param));
	s->sched_priority = 99;

	ret = sched_setscheduler(0, SCHED_RR, s);
	printf("sched_setscheduler returns: %d\n",ret);
	printf("New scheduling policy: %d\n", sched_getscheduler(0));

	if(ret == -1)
	{
		if(errno == EINVAL)
			printf("EINVAL\n");
		if(errno == EPERM)
			printf("EPERM\n");
		if(errno == ESRCH)
			printf("ESRCH\n");
	}

	return 0;
}
