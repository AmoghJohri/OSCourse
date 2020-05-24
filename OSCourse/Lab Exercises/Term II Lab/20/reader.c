#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // opening the file for reading
    int fd = open("myfifo", O_RDONLY);
    if(fd == -1)
    {
        printf("myfifo failed to opne!\n");
        return -1;
    }
    else
        printf("Fifo file opened successfully!\n");
    while(1)
    {
        char* buffer = (char*)calloc(80, sizeof(char));
        read(fd, buffer, sizeof(buffer));
        printf("%s\n", buffer);
    }
    close(fd);
}