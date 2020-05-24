#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("Wrong signature type!\n");
		printf("Usage: gcc store.c <filename>");
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
    
    struct flock* f = (struct flock*) calloc(1, sizeof(struct flock));
    f->l_type = F_WRLCK;
	f->l_start = 0;
	f->l_whence = SEEK_SET;
	f->l_len = 0;
	f->l_pid = getpid();
	
    int ret = fcntl(fd, F_SETLKW, f);
    
    if(ret == 0)
    {
	    char* to_write = (char*) calloc(5, sizeof(char));
	    int size = read(fd, to_write, 5);
        if(size == 0)
	    {
	    	printf("Unable to read into file!\n");
	    }
        int tw = atoi(to_write) + 1;
        sprintf(to_write, "%d", tw); 
        lseek(fd, 0, SEEK_SET);
        size = write(fd, to_write, 5);
        if(size == 0)
	    {
	    	printf("Unable to write into file!\n");
	    }
    }
	
	close(fd);

	return 0;
}
