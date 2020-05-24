#include <stdio.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/sem.h>

int main()
{
    key_t key;
    key = ftok(".", 'a');
    int semid = semget(key, 3, 0);
    printf("Got semaphore with id: %d\n", semid);
    printf("Removing the semaphore!\n");
    if(semctl(semid, 0, IPC_RMID) == 0)
        printf("Successfully removed the semaphore!\n");
    return 0;
}