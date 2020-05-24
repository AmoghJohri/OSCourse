#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[100];    /* message data */
};

int main()
{
    key_t key;
    int msgid;
    key = ftok(".", 'a');
    msgid = msgget(key, 0666 | IPC_CREAT); 

    struct msgbuf msg = {1, "This is my message"};
    msgsnd(msgid, &msg, sizeof(msg), 0);
    
    return 0;

}