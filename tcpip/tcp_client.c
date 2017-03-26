//一般套路 socket connect R/W close
#include<stdio.h>
#include <unistd.h>//read
#include <string.h>//memset
#include <sys/socket.h>//socket connect
#include <arpa/inet.h>//struct sockaddr_in
#include <stdlib.h>

int main(int argc,char *argv[])
{
    int s_sock;//server fd

    struct sockaddr_in s_addr;//server address

    char msg[100];

    if(argc != 3)
    {
        // a.out 127.0.0.1 6666
        fprintf(stderr,"usage:%s <IP> <PORT>",argv[0]);
        exit(1);
    }

    //PF_INET: ipv4
    //SOCK_STREAM: 面向连接，用于tcp
    s_sock = socket(PF_INET,SOCK_STREAM,0);

    if(s_sock == -1)
    {
        perror("socket failed:");
        exit(1);
    }

    memset(&s_addr,0,sizeof(s_addr));

    // a.out 127.0.0.1 6666
    s_addr.sin_family = PF_INET;
    s_addr.sin_port = htons(atoi(argv[2]));
    s_addr.sin_addr.s_addr = inet_addr(argv[1]);//char * 主机字节序转 unsigned int 网络字节序


    if(connect(s_sock,(struct sockaddr *)&s_addr,sizeof(s_addr)) == -1)
    {
        perror("connect failed:");
        exit(1);
    }

    memset(msg,0,sizeof(msg));

    read(s_sock,msg,sizeof(msg));

    fprintf(stdout,"read:%s\n",msg);

    close(s_sock);

    return 0;
}
