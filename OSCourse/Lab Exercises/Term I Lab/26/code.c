#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main()
{
	char* temp[] = {"2.sh", "Amogh", NULL};
	execve("2.sh",temp,NULL);
	printf("Error!\n");
	return 0;
}
