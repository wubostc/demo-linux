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
    char mtext[BUFSIZ]; // the BUFSIZ is 8192 bytes
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
    /*[> Control commands for `msgctl', `semctl', and `shmctl'.  <]*/
    /*#define IPC_RMID	0		[> Remove identifier.  <]*/
    /*#define IPC_SET		1		[> Set `ipc_perm' options.  <]*/
    /*#define IPC_STAT	2		[> Get `ipc_perm' options.  <]*/
    /*#ifdef __USE_GNU*/
    /*# define IPC_INFO	3		[> See ipcs.  <]*/
    /*#endif*/
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

    /*[> Control commands for `msgctl', `semctl', and `shmctl'.  <]*/
    /*#define IPC_RMID	0		[> Remove identifier.  <]*/
    /*#define IPC_SET		1		[> Set `ipc_perm' options.  <]*/
    /*#define IPC_STAT	2		[> Get `ipc_perm' options.  <]*/
    /*#ifdef __USE_GNU*/
    /*# define IPC_INFO	3		[> See ipcs.  <]*/
    /*#endif*/
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
    /*$ ipcs*/
    /*--------- 消息队列 -----------*/
    /*键        msqid      拥有者  权限     已用字节数 消息 */
    key_t key;
    int   msqid;

    if(argc != 2 || ((strcmp(argv[1], "s") && (strcmp(argv[1], "r") ))))
    {
        printf("usage:%s [r || s]\n", argv[0]);
        printf("\t r:receive msg.\n");
        printf("\t s:send msg.\n");
        return 0;
    }

    // 根据文件生成 key
    key = ftok(FILENAME, 1);

    if(key == -1)
    {
        LOGE(ftok failed);
        return -1;
    }

    // IPC_EXCL 不会创建重复的消息
    // get a msqid by the key
    /* Mode bits for `msgget', `semget', and `shmget'.  */
    /*#define IPC_CREAT	01000		[> Create key if key does not exist. <]*/
    /*#define IPC_EXCL	02000		[> Fail if key exists.  <]*/
    /*#define IPC_NOWAIT	04000		[> Return error on wait.  <]*/
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
