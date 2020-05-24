#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>

int main()
{
    char* buffer = (char*)calloc(100, sizeof(char));
    int ret = open("file1", O_RDONLY);
    int size = 0;
    if(ret != -1)
    {
        size = read(ret, buffer, 100);
        if(size != -1)
        {
            printf("Read %d bytes from file1\n", size);
            ret = open("file2", O_WRONLY | O_CREAT);
            if(ret != -1)
            {
                size = write(ret, buffer, size);
                if(size != -1)
                {
                    printf("Written %d bytes into file2\n", size);
                }
                else
                {
                    printf("Unable to write to file2\n");
                }
            }
            else
            {
                printf("Unable to open file2\n");
            }
        }
        else
        {
            printf("Unable to read from file1\n");
        }
    }
    else
    {
        printf("Unable to open file1\n");
    }   
    return 0;
}