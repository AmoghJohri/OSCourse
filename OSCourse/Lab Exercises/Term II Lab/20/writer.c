#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // opening the file for reading
    int fd = open("myfifo", O_WRONLY);
    if(fd == -1)
    {
        printf("myfifo failed to open!\n");
        return -1;
    }
    else
        printf("Fifo file opened successfully!\n");
    while(1)
    {
        char* buffer = (char*)calloc(80, sizeof(char));
        scanf("%s",buffer);
        write(fd, buffer, sizeof(buffer));
    }
    close(fd);
}