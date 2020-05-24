#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
	char* filename = "file1";
	int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0777);
	if(fd == -1)
	{
		fd = open(filename, O_WRONLY);
		if(fd == -1)
		{
			printf("Unable to open file!\n");
			return 0;
		}
	}
	printf("Using dup!\n");
	int new_fd = dup(fd);
	if(new_fd == -1)
	{
		printf("Unable to duplicate the file_descriptor\n");
		return 0;
	}
	int pos = lseek(fd, 0, SEEK_END);
	if(pos == -1)
	{
		printf("lseek failed!\n");
		return 0;
	}
	char* to_write = "Hello World!\n";
	int size = write(fd, to_write, 13);
	int size_dup = write(new_fd, to_write, 13);



	return 0;
}
