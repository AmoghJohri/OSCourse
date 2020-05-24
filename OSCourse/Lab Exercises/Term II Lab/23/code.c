#define _GNU_SOURCE
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
int main()
{
    struct rlimit* rlim = (struct rlimit*)calloc(1, sizeof(struct rlimit));
    getrlimit(RLIMIT_NOFILE, rlim);
    printf("Number of files (soft-limit): %ld\n",rlim->rlim_cur - 4);
    printf("Number of files (hard-limit): %ld\n",rlim->rlim_max - 4);
    
    int pipefd[2];
    pipe(pipefd);
    int read_pipe_size = fcntl(pipefd[0], F_GETPIPE_SZ);
    printf("Size of the read-end of the pipe: %d\n", read_pipe_size);
    int write_pipe_size = fcntl(pipefd[1], F_GETPIPE_SZ);
    printf("Size of the write-end of the pipe: %d\n", write_pipe_size);
    printf("The above sizes should be same!\n");
    return 0;

}