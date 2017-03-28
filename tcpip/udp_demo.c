#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>// define MACRO
#include <arpa/inet.h>


#define SER_PORT 0xaaaa
#define CLT_PORT 0xbbbb
#define SER_IP   "192.168.1.3"


void StartServer()
{
    int ret;
    char buf[1024];
    int fd;
    struct sockaddr_in addr;

    fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        perror("socket failed");
        return;
    }


    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);// 接受任何ip
    addr.sin_port = htons(SER_PORT);

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    if(ret != 0)
    {
        perror("bind failed");
        close(fd);
        return;
    }

    struct sockaddr_in src_addr;
    socklen_t len = sizeof(src_addr);

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = recvfrom(fd, buf, sizeof(buf), 0,
                    (struct sockaddr *)&src_addr, &len);
        buf[ret] = '\0';

        if(ret < 0)
        {
            perror("recvfrom failed");
            break;
        }
        else if(ret == 0)
        {
            fprintf(stderr, "msg: None.\n");
            /*break;*/
        }
        else
        {
            if(strcmp(buf, "exit") == 0)
            {
                puts("bye~");
                continue;
            }
            fprintf(stdout, "recvfrom:%s:%d:%s\n",
                        inet_ntoa(src_addr.sin_addr),
                        ntohs(src_addr.sin_port),
                        buf);

            strcat(buf, " [OK]");

            sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&src_addr, len);
        }

    }
}

void StartClient()
{
    int fd;
    struct sockaddr_in addr;
    int ret;
    char buf[1024];

    fd = socket(PF_INET, SOCK_DGRAM, 0);

    if(fd == -1)
    {
        perror("socket failed");
        return;
    }

    // 与客户端端口进行绑定，并接受任何IP的socket
    addr.sin_family = AF_INET;
    addr.sin_port = htons(CLT_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    if(ret == -1)
    {
        perror("bind failed");
        close(fd);
        return;
    }

    // server info
    struct sockaddr_in target_addr;
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(SER_PORT);
    target_addr.sin_addr.s_addr = inet_addr(SER_IP);

    while(1)
    {
        memset(buf, 0, sizeof(buf));

        fprintf(stdout, "sendto:");

        fgets(buf, sizeof(buf), stdin);

        buf[strlen(buf) - 1] = '\0';// e.g. {'a','b','c','\n'} -> {'a','b','c','\0'}


        ret = sendto(fd, buf, strlen(buf), 0,
                    (struct sockaddr *)&target_addr, sizeof(target_addr));

        if(strcmp(buf, "exit") == 0)
        {
            break;
        }

        if(ret == -1)
        {
            perror("sendto failed");
            break;
        }
        else
        {

            socklen_t len = sizeof(target_addr);

            ret = recvfrom(fd, buf, sizeof(buf), 0,
                        (struct sockaddr *)&target_addr, &len);


            if(ret == -1)
            {
                perror("recvfrom failed");
                break;
            }

            buf[ret] = '\0';
            fprintf(stdout, "recvfrom:%s\n", buf);
        }

    }

    close(fd);
}

int main(int argc, char **argv)
{
    if(argc != 2 || (!strcmp(argv[1],"s") && !strcmp(argv[1],"c")))
    {
       fprintf(stdout,"usage: %s [s | c]\n", argv[0]);
       fprintf(stdout,"\ts: server\n");
       fprintf(stdout,"\tc: client\n");
       return 0;
    }

    if(argv[1][0] == 's')
    {
        StartServer();
    }
    else
    {
        StartClient();
    }

    return 0;
}
