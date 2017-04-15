#include <stdio.h>
#include <sys/msg.h> // msgget

#define DEBUG
#include "../comm.h"

// $ ls
// $ ... a ...
#define FILENAME "a"



int main()
{
    int msqid;

    //IPC_PRIVATE key=0 可以同时存在多个
    //  parent and child process communication
    msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0707);

    LOGI(msqid=%d,msqid);

    if(msqid == -1)
    {
        LOGE(ftok failed);
        return 1;
    }

    msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0707);

    LOGI(msqid=%d,msqid);

    if(msqid == -1)
    {
        LOGE(msgget failed);
        return 1;
    }


    return 0;
}
