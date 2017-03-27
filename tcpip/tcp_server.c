// socket bind listen accpet WR close
/*创建socket——绑定地址（IP地址+端口地址）——监听连接请求——接受连接请求——进行数据交换*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>//exit
#include <sys/socket.h>
#include <arpa/inet.h>// sockaddr_in
#include <unistd.h>// write
int main(int argc,char *argv[])
{
    int s_sock;// server fd
    int c_sock;// client fd
    struct sockaddr_in s_addr;
    struct sockaddr_in c_addr;
    socklen_t c_size;
    char msg[] = "hi";
    char buf[100];

    if(argc != 2)
    {
        fprintf(stderr,"usage:%s <port>\n",argv[0]);
        exit(1);
    }

    //PF_INET: ipv4
    //SOCK_STREAM: 面向连接
    s_sock = socket(PF_INET,SOCK_STREAM,0);

    if(s_sock == -1)
    {
        perror("socket failed");
        exit(1);
    }

    memset(&s_addr,0,sizeof(s_addr));

    s_addr.sin_family = AF_INET;//AF: address family
    s_addr.sin_port = htons(atoi(argv[1]));//主机字节序转网络字节序，端口
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);//主机字节序转网络字节序，ip地址

    if(bind(s_sock,(struct sockaddr *)&s_addr,sizeof(s_addr)) == -1)
    {
        perror("bind failed:");
        close(s_sock);
        exit(1);
    }

    if(listen(s_sock,5) == -1)
    {
        perror("listen failed");
        close(s_sock);
        exit(1);
    }

    c_size = sizeof(c_addr);

    c_sock = accept(s_sock,(struct sockaddr *)&c_addr,&c_size);

    if(c_sock == -1)
    {
        perror("accept failed:");
        close(s_sock);
        exit(1);
    }

    memset(buf,0,100);
    read(c_sock,buf,100);

    printf("read:%s\n",buf);

    write(c_sock,msg,strlen(msg));

    close(s_sock);
    close(c_sock);

    return 0;
}
