#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6670

void StartConnect()
{
    struct sockaddr_in addr;
    int fd;
    char msg[] = "hello";
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

    write(fd, msg, strlen(msg));

    memset(buf, 0, sizeof(buf));

    read(fd, buf, sizeof(buf));

    printf("read: %s\n", buf);

    close(fd);
}

int main()
{
    StartConnect();
    return 0;
}
