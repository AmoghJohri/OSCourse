#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
	if(creat("file1", 0777) != 0)
		printf("file1 created!\n");
	if(creat("file2", 0777) != 0)
		printf("file2 created!\n");
	if(creat("file3", 0777) != 0)
		printf("file3 created!\n");
	if(creat("file4", 0777) != 0)
		printf("file4 created!\n");
	if(creat("file5", 0777) != 0)
		printf("file5 created!\n");
	
	printf("Going into an infinite loop!\n");
	while(1){}
	return 0;
}
