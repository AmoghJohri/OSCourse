#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main()
{
	char* temp[] = {"ls","-Rl",NULL};
	execv("/bin/ls", temp);
	printf("Error!\n");
	return 0;
}
