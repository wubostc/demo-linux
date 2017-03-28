#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 0xeeec

void StartConnect()
{
    struct sockaddr_in addr;
    int fd;
    char buf[1024];

    fd = socket(PF_INET, SOCK_STREAM, 0);// use IPV4 protocol

    if(fd < 0)
    {
        perror("socket failed");
        close(fd);
        return;
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; // use IPV4 addr
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(PORT);

    if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("connect failed");
        close(fd);
        return;
    }

    // 开始交互
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        fprintf(stdout, "send:");
        fgets(buf, sizeof(buf), stdin);

        /*write(fd, buf, strlen(buf));*/
        send(fd, buf, strlen(buf), 0);// send 网络专用 [>MSG_CMSG_CLOEXEC<]

        if((strcmp(buf,"exit\n") == 0) || (strcmp(buf, "killserver\n") == 0))
        {
            break;
        }

        memset(buf, 0, sizeof(buf));
        /*read(fd, buf, sizeof(buf));*/
        recv(fd, buf, sizeof(buf), 0);// recv 网络专用

        printf("read:%s", buf);
    }

    close(fd);
}

int main()
{
    StartConnect();
    return 0;
}
