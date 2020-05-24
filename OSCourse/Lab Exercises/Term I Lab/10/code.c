#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	char* filename = "file1.od";
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
	char* to_write = "HelloWorld";
	int size = write(fd, to_write, 10);
	if(size != 10)
	{
		printf("Unable to write!\n");
		return 0;
	}
	int new_pos = lseek(fd, 10, SEEK_CUR);
	if(new_pos != 20)
	{
		printf("error in execution of lseek!\n");
		return 0;
	}
	size = write(fd, to_write, 10);
	if(size != 10)
	{
		printf("Unable to write!\n");
		return 0;
	}
	printf("Value of lseek is: %d\n", new_pos);
	
	close(fd);
	fd = open(filename, O_RDONLY);
	if(fd == -1)
	{
		printf("Unable to open the file!\n");
		return 0;
	}
	char buffer[1];
	int empty = 0;
	while(buffer != 'EOF')
	{
		size = read(fd, buffer, sizeof(buffer));
		if(size == 0)
			break;

		if((int)buffer[0] == 0)
		empty = empty + 1;
		printf("%c",buffer[0]);;
	}
	printf("\n");
	printf("Number of empty spaces are: %d\n",empty);
	close(fd);
	return 0;
}

