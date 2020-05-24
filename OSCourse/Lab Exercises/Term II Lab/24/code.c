#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <stdlib.h>


int main()
{
    key_t key; 
    int msgid; 
  
    key = ftok(".", 'a'); 
  
    msgid = msgget(key, 0666 | IPC_CREAT); 

    printf("Key for the message queue: %d\n", key);
    printf("Id of the message queue: %d\n", msgid);
    return 0;   
}