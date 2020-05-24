#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

int main()
{
	char* filename = "file";
	int fd = open(filename, O_CREAT | O_EXCL | O_RDWR, 0777);
	if(fd == -1)
	{
		fd = open(filename, O_RDWR);
		if(fd == -1)
		{
			printf("Unable to open the file!\n");
			return 0;
		}
	}
	int opening_mode = fcntl(fd, F_GETFL);
	int accmode = opening_mode & O_ACCMODE;
    	if      (accmode == O_RDONLY)   printf("read only");
    	else if (accmode == O_WRONLY)   printf("write only");
    	else if (accmode == O_RDWR)     printf("read write");
    	else printf("unknown access mode");
	printf("\n");
	return 0;
}
