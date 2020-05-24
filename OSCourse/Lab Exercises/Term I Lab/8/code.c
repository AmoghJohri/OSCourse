#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char buffer[1];

int main()
{
	int fd = open("file1", O_RDONLY);
	if(fd == -1)
	{
		printf("Unable to open the file!\n");
		exit(0);
	}
	while(buffer != 'EOF')
	{
		while(buffer != '\n')
		{
			int size = read(fd, buffer, sizeof(buffer));
			if(size != 1)
			{
				return 0;
			}
			printf("%s",buffer);
		}
	}

	return 0;
}
