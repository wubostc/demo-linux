#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h> // msgget
#include <sys/ipc.h>
#include <string.h>

#define DEBUG
#include "../comm.h"

// $ ls
// $ ... a ...
#define FILENAME "a"


typedef struct
{
    long mtype;
    char mtext[BUFSIZ];
} msg_info;

int rcv_msg(int msqid, long type, char *buf)
{
    msg_info msg;
    int ret = msgrcv(msqid, &msg, BUFSIZ, type, 0);

    if(ret == -1)
    {
        LOGE(msgrcv failed);
        return -1;
    }

    strcpy(buf, msg.mtext);

    return 0;
}

int main()
{
    key_t key;
    int   msqid;

    key = ftok(FILENAME, 1);

    if(key == -1)
    {
        LOGE(ftok failed);
        return -1;
    }

    msqid = msgget(key, IPC_CREAT /*| IPC_EXCL*/ | 0600);

    if(msqid == -1)
    {
        LOGE(msgget failed);
        return -1;
    }

    char buf[BUFSIZ] = {0};

    rcv_msg(msqid, 1, buf);

    LOGI(buf:%s,buf);

    return 0;
}
