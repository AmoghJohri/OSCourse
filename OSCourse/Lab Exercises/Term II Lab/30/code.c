#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void read_data(char* pointer, int size)
{
    for(int i = 0; i < size; i++)
        printf("%c ", *(pointer+i));
    printf("\n");
    return ;
}

int main()
{
    key_t key;
    int rand;
    key = ftok(".",'a');
    int size = 1024;
    int shmid = shmget(key + rand%10,size,0666|IPC_CREAT); 
    printf("ID for the shared memory is: %d\n", shmid);
    char *pointer = (char*) shmat(shmid,(void*)0,0); 
    printf("Reading the data before writing anything!\n");
    read_data(pointer, size);
    sleep(2);
    for(int i = 0; i < 10; i++)
        *(pointer + i) = 'a';
    printf("Reading the data after writing!\n");
    read_data(pointer, size);
    char* pointer_read_only = (char*)shmat(shmid, (void*)0, 0 | SHM_RDONLY);
    printf("Attached with a readonly pointer!\n");
    printf("Attempting to overwrite...\n");
    sleep(2);
    // for(int i = 0; i < 10; i++)
    //     *(pointer_read_only + i) = 'b';
    printf("I have commented out the next two lines where we attempt to overwrite using a read_only pointer, which eventually results in a seg fault!\n");
    printf("Reading after an overwrite attempt!\n");
    read_data(pointer_read_only, size);
    printf("Detaching the memory segments...\n");
    int val = shmdt(pointer);
    if(val != 0) printf("Detach failed!\n");
    val = shmdt(pointer_read_only);
    if(val != 0) printf("Detach failed!\n");
    printf("Removing the shared memory...\n");
    shmctl(shmid, IPC_RMID,NULL);
    return 0;
}