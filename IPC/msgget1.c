#include <stdio.h>
#include <sys/types.h> //key_t
#include <sys/ipc.h>   //ftok
#include <sys/msg.h>

#define DEBUG
#include "../comm.h"

// $ ls
// $ ... a ...
#define FILENAME "a"



int main()
{
    key_t key;
    int msqid;

    key = ftok(FILENAME, 1);

    LOGI(key=%d,key);

    if(key == -1)
    {
        LOGE(ftok failed);
        return 1;
    }

    /*IPC_CREAT: Create key if key does not exist.*/
    msqid = msgget(key, IPC_CREAT | 0707);

    LOGI(msqid=%d,msqid);

    if(msqid == -1)
    {
        LOGE(msgget failed);
        return 1;
    }


    return 0;
}
