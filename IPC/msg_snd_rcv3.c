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
        memset(buf,0,BUFSIZ);

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



/*--------- 消息队列 -----------*/
/*键        msqid      拥有者  权限     已用字节数 消息     */
void print_perm(struct ipc_perm *p)
{
    printf("-------------------IPC_PERM-----------------\n");
    printf("key   :%#x\n", p->__key);
    printf("uid   :%d\n", p->uid);
    printf("gid   :%d\n", p->gid);
    printf("cuid  :%d\n", p->cuid);
    printf("cgid  :%d\n", p->cgid);
    printf("mode  :%#o\n", p->mode);
    printf("seq   :%d\n", p->__seq);
    printf("--------------------------------------------\n");
}

void print_msg(struct msqid_ds *p)
{
    printf("------------------MSG_QUEUE-----------------\n");
    printf("snd time  :%ld\n",p->msg_stime);/* time of last msgsnd command */
    printf("rcv time  :%ld\n",p->msg_rtime);/* time of last msgrcv command */
    printf("chang time:%ld\n",p->msg_ctime);/* time of last change */
    printf("que msglen:%ld\n",p->__msg_cbytes);/* current number of bytes on queue */
    printf("que msgnum:%ld\n",p->msg_qnum);/* number of messages currently on queue */
    printf("que msgmaxlen:%ld\n",p->msg_qbytes);/* max number of bytes allowed on queue */
    printf("queLastSndPid:%d\n",p->msg_lspid);/* pid of last msgsnd() */
    printf("queLastRcvPid:%d\n",p->msg_lrpid);/* pid of last msgrcv() */
    printf("--------------------------------------------\n");
}

int check_mode(char *mode)
{
    int i;
    int m = 0;
    if(strlen(mode) != 3) return 0;

    for(i = 0; i<3; ++i)
    {
        if(mode[i] < '0' || mode[i] > '7') return 0;

        // oct to dec
        m = m * 8 + (mode[i] - '0');
    }

    return m;
}

#define SHOW_UAGE \
    do{  \
    printf("\t perm: output ipc_perm\n");    \
    printf("\t msqinfo: output msqid_ds\n"); \
    printf("\t set:     set mode\n");     \
    printf("\t q:exit\n"); \
    }while(0)

void print_info(int msqid)
{
    char cmd[100];
    struct msqid_ds msq;

    SHOW_UAGE;

    while(1)
    {
        fgets(cmd, 100, stdin);
        cmd[strlen(cmd) - 1] = 0;
        if(strcmp(cmd,"perm") == 0)
        {
            msgctl(msqid, IPC_STAT, &msq);
            print_perm(&msq.msg_perm);
        }
        else if(strcmp(cmd,"msqinfo") == 0)
        {
            msgctl(msqid, IPC_STAT, &msq);
            print_msg(&msq);
        }
        else if(strcmp(cmd,"q") == 0)
        {
            break;
        }
        else if(strcmp(cmd,"set") == 0)
        {
            char mode[100];
            memset(mode,0,100);
            scanf("%s",mode);
            int m = check_mode(mode);
            if(m == 0)
            {
                LOGE(check_mode failed);

            }
            else
            {
                msgctl(msqid,IPC_STAT, &msq);
                msq.msg_perm.mode = m;

                int ret = msgctl(msqid, IPC_SET, &msq);

                if(ret == 0)
                {
                    LOGI(success set new mode %#o, m);
                }
                else
                {
                    LOGE(msgctl failed);
                }

            }
        }
        else
        {
            SHOW_UAGE;
        }
    }
}

int main(int argc, char *argv[])
{
    /*$ ipcs*/
    /*--------- 消息队列 -----------*/
    /*键        msqid      拥有者  权限     已用字节数 消息 */
    key_t key;
    int   msqid;

    if(argc != 2 || (strcmp(argv[1], "s") &&
                    strcmp(argv[1], "r") &&
                    strcmp(argv[1], "i")))
    {
        printf("usage:%s [r | s | i]\n", argv[0]);
        printf("\t r:receive msg.\n");
        printf("\t s:send msg.\n");
        printf("\t i:queue info.\n");
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
    else if(argv[1][0] == 'i')
    {
        print_info(msqid);
    }



    return 0;
}
