#include<stdio.h>
#include<stdlib.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
	struct timeval* timeout = (struct timeval*) calloc(1, sizeof(struct timeval));
	timeout->tv_sec = 5; // giving a timeout of 5 seconds
	timeout->tv_usec = 0;
	
	int fd = 0; // STDIN
	fd_set* fdset = (fd_set*)calloc(1,sizeof(fd_set));
	FD_ZERO(fdset);
	FD_SET(fd, fdset);
	fd_set* empty = (fd_set*)calloc(1,sizeof(fd_set));
	FD_ZERO(empty);
	int t = select(1, fdset, empty, empty, timeout);
	if(t == 0)
	{
		printf("No data is available!\n");
		return 0;
	}
	printf("Data is available\n");

	return 0;

}
