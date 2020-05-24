#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/msg.h>

int main()
{
    // creating a message queue
    key_t key;
    int msgid;
    key = ftok(".",'a');
    msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid == -1)
        printf("Failed to create the message quque!\n");
    sleep(3); // check here using ipcs -q about the existing message queues
    // removing this message queue
    int val = msgctl(msgid, IPC_RMID, NULL);
    if(val == -1)
        printf("Failed to remove the message queue!\n");
    return 0;
}