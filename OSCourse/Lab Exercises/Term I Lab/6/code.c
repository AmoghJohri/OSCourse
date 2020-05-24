#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>



int main()
{
    char* buffer = (char*) calloc(10, sizeof(char));
    int size_read = read(0, buffer, 10); // reading a maximum of 10 bytes
    int size_write = 0;
    if(size_read > 0)
    {
        printf("Read %d bytes from STDIN\n", size_read);
        size_write = write(1, buffer, size_read);
        if(size_write == 0)
        {
            printf("Failed to write anything\n");
        }
        else
        {
            printf("Wrote %d bytes to STDOUT\n",size_write);
        }
    }
    else
    {
        printf("Failed to read anything\n");
    }
    
    
    return 0;
}