#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>// INADDR_ANY
#include <unistd.h>

#define PORT 6670

void StartServer()
{
    struct sockaddr_in addr;
    struct sockaddr_in src_addr;
    int fd;
    socklen_t len;
    int client_fd;

    char buf[1024];
    char msg[] = "hi";


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
        client_fd = accept(fd, (struct sockaddr *)&src_addr, &len);
        if(client_fd == -1)
        {
            perror("accept failed");
            break;
        }

        memset(buf,0,sizeof(buf));

        if(read(client_fd,buf,sizeof(buf)) == -1)
        {
            perror("read failed");
            break;
        }
        else
        {
            printf("read: %s\n", buf);
            write(client_fd, msg, strlen(msg));
        }

        close(client_fd);
    }

    close(fd);
}

int main()
{
    StartServer();
    return 0;
}
