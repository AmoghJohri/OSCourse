#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

struct msgbuf 
{
    int msgtype;
    char msgbuf[100];
};

int main()
{
    key_t key;
    key = ftok(".",'a');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msgbuf mymsg = {1, "This is my message!"};
    printf("Sending the message...\n");
    msgsnd(msgid, &mymsg, sizeof(struct msgbuf), 0);
    return 0;
}