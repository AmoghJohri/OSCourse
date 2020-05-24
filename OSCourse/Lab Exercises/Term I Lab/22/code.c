#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	fork();
	char* to_write = "Hello World!\n";
	int ret = write(fd, to_write, strlen(to_write));
	if(ret != strlen(to_write))
	{
		printf("Unable to write to file!\n");
	}
	close(fd);
	return 0;
}
