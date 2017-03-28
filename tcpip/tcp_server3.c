#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>// INADDR_ANY
#include <unistd.h>
#include <arpa/inet.h>// inet_ntoa

#define PORT 0xeeec

void StartServer()
{
    struct sockaddr_in addr;
    struct sockaddr_in src_addr;
    int fd;
    socklen_t len;
    int client_fd;

    char buf[1024];


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

        while(1)
        {
            int ret;

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
                fprintf(stdout, "[%d] fd msg: None\n", client_fd);
                break;
            }
            else // ret > 0
            {
                if(strcmp(buf, "exit\n") == 0)
                {
                    puts("bye~");
                    break;
                }
                if(strcmp(buf, "killserver\n") == 0)
                {
                    close(client_fd);
                    goto END;
                }

                printf("read:%s", buf);
                buf[strlen(buf) - 1] = '\0';
                strcat(buf," [OK]\n");
                /*write(client_fd, buf, strlen(buf));*/
// MSG_MORE 表示后面还有数据要发送，不马上发送
                send(client_fd, buf, strlen(buf), MSG_MORE);
                send(client_fd, buf, strlen(buf), 0);// 网络专用
            }
        }
        close(client_fd);
    }

END:
    close(fd);
}

int main()
{
    StartServer();
    return 0;
}
