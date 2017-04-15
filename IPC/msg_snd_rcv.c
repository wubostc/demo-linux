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

void StartSend(int msqid)
{
    long type;
    char buf[BUFSIZ];

    while(1)
    {
        fprintf(stderr, "Send [type msg]:");

        scanf("%lu %s", &type, buf);

        if(type == 0) break;

        snd_msg(msqid, type, buf);
    }
}

void StartRecv(int msqid)
{
    long type;
    char buf[BUFSIZ];

    while(1)
    {
        fprintf(stderr, "Recv:");

        scanf("%lu",&type);

        if(type == 0) break;

        rcv_msg(msqid, type, buf);

        printf(" %s\n", buf);
    }
}

int main(int argc, char *argv[])
{
    key_t key;
    int   msqid;

    if(argc != 2 || ((strcmp(argv[1], "s") && (strcmp(argv[1], "r") ))))
    {
        printf("usage:%s [r || s]\n", argv[0]);
        printf("\t r:receive msg.\n");
        printf("\t s:send msg.\n");
        return 0;
    }

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

    if(argv[1][0] == 's')
    {
        StartSend(msqid);
    }

    else if(argv[1][0] == 'r')
    {
        StartRecv(msqid);
    }



    return 0;
}
