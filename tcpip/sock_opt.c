#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 0xa39d


// 数据包组成 [ [IP头][协议][用户数据] ]

typedef struct
{
    /* 下面共 20 个字节 */
    unsigned char ver:4;        //4位版本
    unsigned char head_len:4;   //4位首部长度
    unsigned char TOS;          //8位服务类型
    unsigned short byte_len;    //16位总长度
    unsigned short ident;       //16位标识位
    unsigned short flag:3;      //3位标志
    unsigned short offset:13;   //13位片偏移
    unsigned char TTL;          //8位生存时间
    unsigned char protocol;     //8位协议
    unsigned short chksum;      //16位首部校验和
    unsigned int src_ip;        //32位源IP地址
    unsigned int dst_ip;        //32位目的IP地址
    /* 上面共 20 个字节 */


    /* 柔性数组 sizeof(IP_HEADER) == 20 */
    unsigned char data[0];      //数据
    /* 柔性数组 sizeof(IP_HEADER) == 20 */

} IP_HEADER;


void PrintIpHead(char *buf)
{
    IP_HEADER *p = (IP_HEADER *)buf;
    printf("---------------IP HEAD-------------------\n");
    printf("Version:%d\n",p->ver);
    printf("HeadLen:%d\n",p->head_len * 4);
    printf("TOS    :%d\n",p->TOS);
    printf("DataLen:%d\n",ntohs(p->byte_len));
    printf("Ident  :%d\n",ntohs(p->ident));
    printf("Flag   :%d\n",p->flag);
    printf("Offset :%d\n",p->offset);// 不要转
    printf("TTL    :%d\n",p->TTL);
    printf("Proto  :%d\n",p->protocol);
    printf("Chksum :%d\n",p->chksum);
    printf("SrcIP  :%s\n",inet_ntoa(*(struct in_addr *)&(p->src_ip)));
    printf("DstIP  :%s\n",inet_ntoa(*(struct in_addr *)&(p->dst_ip)));
    printf("Data   :%p\n",(void *)p->data);// 后面跟具体协议
    printf("-----------------------------------------\n");
}

//use udp
void Capture_Data()
{
    int fd;
    struct sockaddr_in addr;
    int ret;
    char buf[1024];

    /*原始套接字(SOCK_RAW)是一种不同于SOCK_STREAM、SOCK_DGRAM的套接字，*/
    /*它实现于系统核心。然而，原始套接字能做什么呢？*/
    /*首先来说，普通的套接字无法处理ICMP、IGMP等网络报文，而SOCK_RAW可以；*/
    /*其次，SOCK_RAW也可以处理特殊的IPv4报文；此外，利用原始套接字，*/
    /*可以通过IP_HDRINCL套接字选项由用户构造IP头。*/
    /*总体来说，SOCK_RAW可以处理普通的网络报文之外，*/
    /*还可以处理一些特殊协议报文以及操作IP层及其以上的数据。*/
    /*fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);// ICMP 1 ping */
    /*fd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);// TCP   6*/
    fd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);// UDP   17


    if(fd < 0)
    {
        perror("socket failed");
        return;
    }

    int opt = 1; 
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind failed");
        close(fd);
        return;
    }

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = recv(fd, buf, sizeof(buf), 0);

        if(ret == -1)
        {
            perror("recv failed");
            break;
        }
        else if(ret > 0)
        {
            /*fprintf(stdout, "recv:%s\n", buf);*/
            PrintIpHead(buf);
        }
        else // 0 byte
        {
            fprintf(stderr, "msg: None\n");
            break;
        }
    }

    close(fd);
}


int main()
{
    Capture_Data();

    return 0;
}
