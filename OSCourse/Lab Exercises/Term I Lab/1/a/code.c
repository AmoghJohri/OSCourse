#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>

int main()
{
    printf("Symlink system-call returns: %d\n",symlink("test_file","sym_link"));
    return 0;
}