#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
	char* file_to_create = "created_file";
	creat(file_to_create, 0777);

	printf("Opening an existing file in Read/Write more\n");
	int ret = open(file_to_create, O_RDWR);
	if(ret == -1)
		printf("Unable to open the file!\n");
	else
		printf("File descriptor of the open file is: %d\n",ret);

	printf("Trying the O_EXCL flag\n");
	int ret1 = open(file_to_create, O_RDWR | O_CREAT | O_EXCL);
	printf("Using the O_EXCL flag for a file which already exists returns: %d\n", ret1);
	int ret2 = open("new_file", O_RDWR | O_CREAT | O_EXCL);
	printf("Using the O_EXCL flag to create a new file returns: %d\n", ret2);
	return 0;
}
