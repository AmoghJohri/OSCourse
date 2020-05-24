#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("Wrong input signature!\n");
		printf("Use - gcc code.c <filename>\n");
	}
	char* filename = argv[1];
	struct stat* buff = (struct stat*) calloc(1, sizeof(struct stat));
	
	int ret = lstat(filename, buff);
	if(ret == -1)
	{
		printf("lstat execution failed!\n");
		return 0;
	}
	printf("File type is: ");	
	switch (buff->st_mode & S_IFMT) {
           case S_IFBLK:  printf("block device\n");            break;
           case S_IFCHR:  printf("character device\n");        break;
           case S_IFDIR:  printf("directory\n");               break;
           case S_IFIFO:  printf("FIFO/pipe\n");               break;
           case S_IFLNK:  printf("symlink\n");                 break;
           case S_IFREG:  printf("regular file\n");            break;
           case S_IFSOCK: printf("socket\n");                  break;
           default:       printf("unknown?\n");                break;
           }

	return 0;
}
