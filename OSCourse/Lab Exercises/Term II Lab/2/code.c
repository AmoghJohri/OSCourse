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

    check(getrlimit(RLIMIT_AS, buffer));
    printf("Soft limit on process's virtual memory: %ld\n", buffer->rlim_cur);
    printf("Hard limit on process's virtual memory: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_CORE, buffer));
    printf("Soft limit on maximum size of core file: %ld\n", buffer->rlim_cur);
    printf("Hard limit on maximum size of core file: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_CPU, buffer));
    printf("Soft limit on process's CPU time consumption: %ld\n", buffer->rlim_cur);
    printf("Hard limit on process's CPU time consumption: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_DATA, buffer));
    printf("Soft limit on process's data segment: %ld\n", buffer->rlim_cur);
    printf("Hard limit on process's data segment: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_LOCKS, buffer));
    printf("Soft limit on flocl locks + fcntl leases: %ld\n", buffer->rlim_cur);
    printf("Hard limit on flocl locks + fcntl leases: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_MEMLOCK, buffer));
    printf("Soft limit on bytes of memory that can be locked into RAM: %ld\n", buffer->rlim_cur);
    printf("Hard limit on bytes of memory that can be locked into RAM: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_MSGQUEUE, buffer));
    printf("Soft limit on bytes for message queues: %ld\n", buffer->rlim_cur);
    printf("Hard limit on bytes for message queues: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_NICE, buffer));
    printf("Soft limit on process's nice value: %ld\n", buffer->rlim_cur);
    printf("Hard limit on process's nice value: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_NPROC, buffer));
    printf("Soft limit on number of extant process: %ld\n", buffer->rlim_cur);
    printf("Hard limit on number of extant process: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_NOFILE, buffer));
    printf("Soft limit on maximum file-descriptor number: %ld\n", buffer->rlim_cur - 1);
    printf("Hard limit on maximum file-descriptor number: %ld\n", buffer->rlim_max - 1);

    check(getrlimit(RLIMIT_RSS, buffer));
    printf("Soft limit on process's resident set: %ld\n", buffer->rlim_cur);
    printf("Hard limit on process's resident set: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_RTPRIO, buffer));
    printf("Soft limit on real-time priority: %ld\n", buffer->rlim_cur);
    printf("Hard limit on real-time priority: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_RTTIME, buffer));
    printf("Soft limit on process's time under real-time scheduling: %ld\n", buffer->rlim_cur);
    printf("Hard limit on process's time under real-time scheduling: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_SIGPENDING, buffer));
    printf("Soft limit on maximum number of siganls to be queued: %ld\n", buffer->rlim_cur);
    printf("Hard limit on maximum number of siganls to be queued: %ld\n", buffer->rlim_max);

    check(getrlimit(RLIMIT_STACK, buffer));
    printf("Soft limit on maximum stack on process stack: %ld\n", buffer->rlim_cur);
    printf("Hard limit on maximum stack on process stack: %ld\n", buffer->rlim_max);

    return 0;
}