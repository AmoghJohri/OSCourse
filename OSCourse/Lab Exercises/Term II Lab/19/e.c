#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <stdio.h>

int main()
{
    if(mkfifo("myfifo_mkfifo", 0) != 0)
        printf("Failed to execute mknod!\n");
    return 0;
}
