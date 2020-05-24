#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main()
{
    char *val = getenv("USER");
    printf("New value of environment variable USER is: %s\n",val);
    return 0;
}