#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void init(char* pointer, int size)
{
    for(int i = 0; i < size; i++)
        *(pointer+i) = '\0';
}

int main()
{
    key_t key, key1, key2;
    key = ftok(".", 's');
    key1 = key + 1; // pseudoresources
    key2 = key + 2; // pseudoresources
    int size = 1024;
    int shmid = shmget(key,size,0666|IPC_CREAT); 
    int shmid1 = shmget(key1,size,0666|IPC_CREAT); 
    int shmid2 = shmget(key2,size,0666|IPC_CREAT); 
    printf("ID for the shared memory is: %d\n", shmid);
    printf("ID for the shared memory is: %d\n", shmid1);
    printf("ID for the shared memory is: %d\n", shmid2);
    char *pointer = (char*) shmat(shmid,(void*)0,0); 
    char *pointer1 = (char*) shmat(shmid1,(void*)0,0); 
    char *pointer2 = (char*) shmat(shmid2,(void*)0,0); 
    printf("Initializing the segment!\n");
    init(pointer, size);
    init(pointer1, size);
    init(pointer2, size);
    return 0;
}