#include <stdio.h>
#include <sys/sem.h>

union semun {
    int val;
    unsigned short int* array;
} arg;

int main()
{
    key_t key;
    key = ftok(".",'a');
    
    printf("Creating a set of 2 semaphores...\n");
    static ushort semarray[3]={1,2,3}; // since we are going to create 1 binary and 2 counting semaphore
    int semid = semget(key,3,IPC_CREAT|0666);
    printf("Making the first semaphore in the set a binary semaphore...\n");
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
    printf("Making the second semaphore in the set a counting semaphore...\n");
    arg.val = 2;
    semctl(semid, 1, SETVAL, arg);
    semctl(semid, 2, SETVAL, arg);
    return 0;
}