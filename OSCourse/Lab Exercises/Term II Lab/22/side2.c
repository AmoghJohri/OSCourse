#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>

int main()
{
    int fd_write = open("fifo", O_WRONLY);
    if(fd_write == -1)
    {
        printf("Fifo (write) file failed to open!\n");
    }
    else
        printf("Fifo (write) file opened!\n");
    
    char* message = "Hello from the other side!";
    sleep(4); // toggle this to see the select doing it's job
    write(fd_write, message, strlen(message));

    close(fd_write);
    return 0;
}