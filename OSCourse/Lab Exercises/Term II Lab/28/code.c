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
    if(val != 0)
    {
        printf("Something went wrong...\n");
        exit(-1);
    }

    struct ipc_perm info_ = info->msg_perm;
    printf("Existing access permissions: %d\n", info_.mode);

    info_.mode ++;
    info->msg_perm = info_;
    
    val = msgctl(msgid, IPC_SET, info);
    if(val != 0)
    {
        printf("Something went wrong...\n");
        exit(-1);
    }
    val = msgctl(msgid, IPC_STAT, info);
    if(val != 0)
    {
        printf("Something went wrong...\n");
        exit(-1);
    }
    info_ = info->msg_perm;
    printf("Changed access permissions: %d\n", info_.mode);

    return 0;

}