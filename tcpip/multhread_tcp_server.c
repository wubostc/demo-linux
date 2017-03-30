#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>// INADDR_ANY
#include <unistd.h>
#include <arpa/inet.h>// inet_ntoa
#include <pthread.h>
#include <semaphore.h>

#define PORT 0xeeec


void *handler(void *param)
{
    char buf[1024];
    int client_fd;
    int ret;

    client_fd = *((int *)param);

    while(1)
    {
        memset(buf, 0, sizeof(buf));

        /*ret = read(client_fd, buf, sizeof(buf));*/
        ret = recv(client_fd, buf, sizeof(buf), 0);// 网络专用
        if(ret == -1)
        {
            perror("read failed");
            break;
        }
        else if (ret == 0)
        {
            fprintf(stdout, "Disconnect\n");
            break;
        }
        else // ret > 0
        {
            if(strcmp(buf, "exit") == 0)
            {
                puts("bye~");
                break;
            }

            if(strcmp(buf, "kill") == 0)
            {
                break;
            }

            printf("read:%s\n", buf);
            buf[ret] = '\0';
            strcat(buf," [OK]");

            send(client_fd, buf, strlen(buf), 0);// 网络专用
        }
    }
    close(client_fd);

    return NULL;
}


void StartServer()
{
    struct sockaddr_in addr;
    struct sockaddr_in src_addr;
    int fd;
    socklen_t len;
    int client_fd;


    //IPPROTO_IP (0) 接受所有ip数据包
    //IPPROTO_TCP    TCP 协议
    //IPPROTO_UDP
    //IPPROTO_RAW
    fd = socket(PF_INET, SOCK_STREAM, 0);// use IPV4 protocol

    if(fd < 0)
    {
        perror("socket failed");
        close(fd);
        return;
    }

    // 重用地址，防止在多次测试中 bind 失败
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET; // use IPV4 addr
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);


    if(bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind failed");
        close(fd);
        return;
    }

    if(listen(fd,10) == -1)
    {
        perror("listen failed");
        close(fd);
        return;
    }

    len = sizeof(src_addr);

    while(1)
    {
        client_fd = accept(fd, (struct sockaddr *)&src_addr, &len);// 得到客户端的地址和端口

        if(client_fd == -1)
        {
            perror("accept failed");
            break;
        }

        fprintf(stdout, "new connect from %s : %d\n",
                    inet_ntoa(src_addr.sin_addr),
                    ntohs(src_addr.sin_port));

        pthread_t id;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//线程退出自动释放资源
        pthread_create(&id, &attr, handler, &client_fd);
    }

    close(fd);
}

int main()
{
    StartServer();
    return 0;
}
