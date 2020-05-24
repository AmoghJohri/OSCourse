#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>

int main()
{
    printf("Link system-call returns: %d\n",link("test_file", "hard_link"));
    return 0;
}