#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

void strreverse(char* begin, char* end) {
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}
	
void itoa(int value, char* str, int base) {
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;
	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }
	// Take care of sign
	if ((sign=value) < 0) value = -value;
	// Conversion. Number is reversed.
	do *wstr++ = num[value%base]; while(value/=base);
	if(sign<0) *wstr++='-';
	*wstr='\0';
	// Reverse string
	strreverse(str,wstr-1);	
}


int read_data(char* pointer, int size)
{
    char* buffer = (char*)calloc(size, sizeof(char));
    for(int i = 0; i < size; i++)
        buffer[i] = *(pointer + i);
    return atoi(buffer);
}

int main()
{
    key_t key, key1, key2;
    key = ftok(".", 's');
    key1 = key + 1;
    key2 = key + 2;
    int size = 1024;
    int shmid = shmget(key,size,0666|IPC_CREAT);
    int shmid1 = shmget(key1,size,0666|IPC_CREAT);
    int shmid2 = shmget(key2,size,0666|IPC_CREAT); 
    printf("ID for the shared memory is: %d\n", shmid);
    printf("ID for the shared memory is: %d\n", shmid1);
    printf("ID for the shared memory is: %d\n", shmid2);
    char *pointer  = (char*) shmat(shmid,(void*)0,0); 
    char *pointer1 = (char*) shmat(shmid1,(void*)0,0); 
    char *pointer2 = (char*) shmat(shmid2,(void*)0,0); 

    int semid = semget(ftok(".",'a'), 3, 0);
    printf("Semaphore Id: %d\n",semid);

    struct sembuf buffer = {0, -1, 0}; // locking buffer

    printf("Reading the data!\n");
    int data = read_data(pointer, size);
    int data1 = read_data(pointer1, size);
    int data2 = read_data(pointer2, size);
    printf("Data: %d\n",data);
    printf("Data: %d\n",data1);
    printf("Data: %d\n",data2);


    semop(semid, &buffer, 1);
    itoa(data+1, pointer, 10); // writing the data (binary semaphore)
    buffer.sem_op = 1;
    semop(semid, &buffer, 1);

    buffer.sem_num = 1;
    buffer.sem_op = -1; // locking a counting semaphore
    semop(semid, &buffer, 1);
    itoa(data1+1, pointer1, 10); // writing the data
    buffer.sem_op = 1;
    semop(semid, &buffer, 1);

    buffer.sem_num = 2;
    buffer.sem_op = -1;
    itoa(data2+1, pointer2, 10); // writing the data
    buffer.sem_op = 1;
    semop(semid, &buffer, 1);

    return 0;
}