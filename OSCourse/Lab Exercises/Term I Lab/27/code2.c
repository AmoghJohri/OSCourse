#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main()
{
        execl("/bin/ls", "ls","-Rl",NULL);
        printf("Error!\n");
        return 0;
}

