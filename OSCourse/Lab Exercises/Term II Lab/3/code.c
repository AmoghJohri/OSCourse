#include <stdio.h> 
#include <sys/resource.h> 
#include <string.h> 
#include <errno.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdlib.h>

void check(int n)
{
    if(n != 0)
    {
        printf("System Call Failed!\n");
        exit(0);
    }
    
}


int main()
{
    struct rlimit* buffer = (struct rlimit*)calloc(1, sizeof(struct rlimit));

    check(getrlimit(RLIMIT_CORE, buffer));
    printf("Soft limit on maximum size of core file: %ld\n", buffer->rlim_cur);
    printf("Hard limit on maximum size of core file: %ld\n", buffer->rlim_max);

    buffer->rlim_cur = 1; // changing the soft-limit for the maximum size of core file from 0 to 1
    check(setrlimit(RLIMIT_CORE, buffer));

    check(getrlimit(RLIMIT_CORE, buffer));
    printf("New Soft limit on maximum size of core file: %ld\n", buffer->rlim_cur);
    printf("New Hard limit on maximum size of core file: %ld\n", buffer->rlim_max);

    return 0;
}