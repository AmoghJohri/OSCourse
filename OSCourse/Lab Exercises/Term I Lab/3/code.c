#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>

int main()
{
	char* file_name = "created_file";
	printf("Name of the file to be created: %s\n",file_name);
	
	int ret = creat(file_name, 0555);
	if(ret != -1)
		printf("File created successfully!\n");
	else
		printf("Unable to create the file!\n");
	return 0;
}
