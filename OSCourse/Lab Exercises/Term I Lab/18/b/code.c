#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
	struct flock* f = (struct flock*) calloc(1, sizeof(struct flock));
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
	printf("Each record is of size 10 bytes!\n");
	printf("Enter which record to lock: ");
	int x;
	scanf("%d",&x);
	x = x * 10;
	printf("Implementing Write Lock!\n");
	f->l_type = F_WRLCK;
	f->l_start = x;
	f->l_whence = SEEK_SET;
	f->l_len = 10;
	f->l_pid = getpid();

	
	int ret = fcntl(fd, F_SETLKW, f);
	printf("Return value of fcntl: %d\n", ret);


	if(ret == 0)
	{
		while(1){
			scanf("%c",NULL); } }
	return 0;
}
