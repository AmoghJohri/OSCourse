#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int pipefd[2];
    int flag;

    printf("Creating a pipe!\n");
    if(pipe(pipefd) == 0)
        printf("Pipe created successfully!\n");
    char* s = "Hello World!\n";
    int msgsize = 16;
    printf("Writing to the pipe!\n");
    flag = write(pipefd[1], s, msgsize);
    if(flag > 0)
        printf("Written to the pipe successfully!\n");
    char* buffer = (char*)calloc(msgsize, sizeof(char));
    printf("Reading from the pipe!\n");
    flag = read(pipefd[0], buffer, msgsize) ;
    if(flag > 0)
    {
        printf("Successfully read from the pipe! The data is: %s", buffer);
    }
    return 0;
}