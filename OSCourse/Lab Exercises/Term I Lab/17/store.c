#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		printf("Wrong signature type!\n");
		printf("Usage: gcc store.c <filename> <value>");
	}
	
	char* filename = argv[1];

	int fd = open(filename, O_CREAT | O_EXCL | O_RDWR, 0777);
	if(fd == -1)
	{
		fd = open(filename, O_RDWR);
		if(fd == -1)
		{
			printf("Unable to open file!\n");
			return 0;
		}
	}
	char* to_write = argv[2];
	int size = write(fd, to_write, strlen(to_write));
	if(size == 0)
	{
		printf("Unable to write into file!\n");
	}
	
	close(fd);

	return 0;
}
