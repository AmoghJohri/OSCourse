#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msgbuf 
{
    int msgtype;
    char msgbuf[100];
};

int main()
{
    key_t key;
    key = ftok(".",'a');

    int msgid = msgget(key, 0666);
    struct msgbuf* mymsg = (struct msgbuf*)calloc(1, sizeof(struct msgbuf));
    printf("This won't wait for the message!\n");
    msgrcv(msgid, mymsg, sizeof(struct msgbuf), 0, IPC_NOWAIT);
    printf("%s\n", mymsg->msgbuf);
    return 0;
}