#include<stdio.h>
#include<sched.h>
int main()
{
	printf("Min runtime priority: %d\n", sched_get_priority_min(SCHED_RR));
	printf("Max runtime priority: %d\n", sched_get_priority_max(SCHED_RR));
}
