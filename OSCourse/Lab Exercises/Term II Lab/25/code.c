#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    key_t key;
    int msgid;
    key = ftok(".", 'a');
    msgid = msgget(key, 0666 | IPC_CREAT); 

    struct msqid_ds* info = (struct msqid_ds*)calloc(1, sizeof(struct msqid_ds));

    int val = msgctl(msgid, IPC_STAT, info);

    struct ipc_perm info_ = info->msg_perm;
    printf("User id: %d\n", info_.uid);
    printf("Group id: %d\n", info_.gid);
    printf("Access permissions: %d\n", info_.mode);
    printf("Time of last message sent: %ld\n", info->msg_stime);
    printf("Time of last message received: %ld\n", info->msg_rtime);
    printf("Time of last change in the queue: %ld\n", info->msg_ctime);
    printf("Number of messages in the queue: %ld\n", info->msg_qnum);
    printf("Current size of the queue: %ld\n", info->__msg_cbytes);
    printf("Allowed size of the queue: %ld\n", info->msg_qbytes);
    printf("PID of last message sent: %d\n", info->msg_lspid);
    printf("PID of last message received: %d\n", info->msg_lrpid);
    return 0;

}