#include <stdio.h>
#include <unistd.h>

int main()
{
    
    printf("a. Maximum length of arguments to the exec family of functions: %ld\n", sysconf(_SC_ARG_MAX));
    printf("b. Maximum number of simultaneous processes over a user id: %ld\n", sysconf(_SC_CHILD_MAX));
    printf("c. Number of clock ticks per-second: %ld\n", sysconf(_SC_CLK_TCK));
    printf("d. Maximum number of files that can be opened by a process at any time: %ld\n", sysconf(_SC_OPEN_MAX));
    printf("e. Page size in bytes: %ld\n", sysconf(_SC_PAGESIZE));
    printf("f. Number of pages of physical memory: %ld\n", sysconf(_SC_PHYS_PAGES));
    printf("g. Number of currently available pages in physical memory: %ld\n", sysconf(_SC_AVPHYS_PAGES));

    return 0;
}