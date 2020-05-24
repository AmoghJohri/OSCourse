#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main()
{
        execlp("/bin/ls", "ls","-Rl",NULL);
        printf("Error!\n");
        return 0;
}

