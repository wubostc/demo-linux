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

int snd_msg(int msqid, long type, char *buf)
{
    int ret;
    msg_info msg;
    msg.mtype = type;
    strcpy(msg.mtext, buf);

    ret = msgsnd(msqid, &msg, strlen(buf), 0);


    if(ret == -1)
    {
        LOGE(msgsnd failed);
        return -1;
    }

    return ret;
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

    msqid = msgget(key, IPC_CREAT | IPC_EXCL | 0600);

    if(msqid == -1)
    {
        LOGE(msgget failed);
        return -1;
    }

    char buf[] = "hello world!";

    snd_msg(msqid, 1, buf);


    return 0;
}
