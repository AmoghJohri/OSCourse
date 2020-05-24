#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int fd_read = open("fifo1", O_RDONLY);
    if(fd_read == -1)
        printf("Fifo (read) file failed to open!\n");
    else
        printf("Fifo (read) file opened!\n");
    int fd_write = open("fifo2", O_WRONLY);
    if(fd_write == -1)
        printf("Fifo (write) file failed to open!\n");
    else
        printf("Fifo (write) file opened!\n");

    printf("Write first!\n");

    while(1)
    {
        char* buffer_read = (char*)calloc(1024, sizeof(char));
        char* buffer_write = (char*)calloc(1024, sizeof(char));

        printf("Message sent: ");
        scanf("%s",buffer_write);
        write(fd_write, buffer_write, sizeof(buffer_write));
        printf("Message recevied: ");
        read(fd_read, buffer_read, sizeof(buffer_read));
        printf("%s\n", buffer_read);
        free(buffer_read);
        free(buffer_write);
    }
    close(fd_read);
    close(fd_write);
    return 0;
}