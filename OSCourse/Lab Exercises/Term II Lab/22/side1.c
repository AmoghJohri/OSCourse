#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>

int main()
{
    int fd_read = open("fifo", O_RDONLY);
    if(fd_read == -1)
    {
        printf("Fifo (read) file failed to open!\n");
        exit(-1);
    }
    else
        printf("Fifo (read) file opened!\n");

    char buffer[1024] = {0};
    printf("It is waiting for something to read...\n");
    
    struct timeval* timer = (struct timeval*)calloc(1, sizeof(struct timeval));
    timer->tv_sec = 3; // setting a timer of 3 seconds instead of 10 as 10 is just too long
    timer->tv_usec = 0;

    fd_set* myset = (fd_set*)calloc(1, sizeof(fd_set));
    FD_ZERO(myset);
    FD_SET(fd_read, myset);
    
    if(select(fd_read + 1, myset, NULL, NULL, timer) == 0)
    {
        printf("No input detected!\n");
        close(fd_read);
        exit(-1);
    };
    read(fd_read, buffer, sizeof(buffer));

    printf("%s\n", buffer);

    close(fd_read);

    return 0;
} 