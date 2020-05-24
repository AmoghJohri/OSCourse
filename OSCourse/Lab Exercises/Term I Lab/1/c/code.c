#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("Creating a named-pipe using mknod: \n");
    printf("mknod system-call returns: %d\n", mknod("pipe_w_mknod", S_IFIFO, 0));

    printf("Creating a named-pipe using mkfifo: \n");
    printf("mkfifo returns: %d\n", mkfifo("pipe_w_mkfifo", 0777));
    return 0;
}