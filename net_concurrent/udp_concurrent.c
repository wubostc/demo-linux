#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/select.h>
#include <sys/time.h>

#include "../comm.h"

#define SER_PORT 0xeeee
#define MAX_CONN 5

void StartServer()
{
    int udp_fd; // socket
    int ret;
    struct sockaddr_in addr;     // server
    struct sockaddr_in src_addr; // client
    socklen_t src_len;           // client
    char buf[1024];  // used to R/W
    int allowed;     // allowed server to REUSEADDR

    // 0 STDIN, 1 STDOUT, 2 STDERR. udp_fd 接下去一般为 3
    // socket(2) SOCK_DGRAM is used to UDP
    udp_fd = socket(PF_INET, SOCK_DGRAM, 0);

    if(udp_fd == -1)
    {
        ELOG(socket failed);
        return;
    }

    allowed = 1;
    setsockopt(udp_fd,
                SOL_SOCKET, SO_REUSEADDR, &allowed, sizeof(allowed));

    // 套路
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SER_PORT);

    ret = bind(udp_fd, (struct sockaddr *)&addr, sizeof(addr));

    if(ret == -1)
    {
        ELOG(bind failed);
        close(udp_fd);
        return;
    }

    fd_set fdset;//for read

    src_len = sizeof(src_addr); // length of client socket

    while(1)
    {
        /* 必须重置 */
        FD_ZERO(&fdset);
        FD_SET(udp_fd, &fdset);
        FD_SET(0, &fdset); // add STDIN_FILENO to fdset

        // select(1) 要监视的 fd 范围，[0, fd + 1)
        // select(2) 关心 fdset 读的变化
        // select(3) 关心 fdset 写的变化
        // select(4) 关心 fdset 错误的变化
        // select(5) 无限等待
        ret = select(udp_fd + 1, &fdset, NULL, NULL, NULL);

        if(ret < 0)
        {
            ELOG(select failed %d, ret);
            break;
        }
        else if(ret == 0)
        {
            //select 无限等待，不可能超时
            LOG(WARN, timeout %d, ret);
            continue;
        }

        if(FD_ISSET(STDIN_FILENO, &fdset))
        {
            LOG(INFO,read from bash...);

            ret = read(STDIN_FILENO, buf, 1024);

            buf[ret - 1] = '\0'; // remove '\n' in exit\n

            if(strcmp(buf, "exit") == 0)
            {
                LOG(INFO, bye~);
                goto END;
            }
            else
            {
                LOG(WARN, command not found!);
                continue;
            }
        }

        // udp_fd 在 feset 中是否可读，表示是否有连接
        // 把连接的 fd 加入 feset 中，然后回到 select
        // 继续等待新的 fd 加入...
        if(FD_ISSET(udp_fd, &fdset))
        {
            // ret is len of bytes
            ret = recvfrom(udp_fd, buf, 1024, 0,
                        (struct sockaddr *)&src_addr, &src_len);

            LOG(INFO,new connection IP:%s,inet_ntoa(src_addr.sin_addr));

            buf[ret] = '\0';

            if(ret == -1)
            {
                ELOG(accept failed %d, ret);
                break;
            }
            else if(ret == 0)
            {
                LOG(INFO, IP: %s has disconnected.,inet_ntoa(src_addr.sin_addr));
                continue;
            }

            if(strcmp(buf, "kill") == 0)
            {
                LOG(WARN, server will exit...);
                goto END;
            }

            LOG(INFO, read:%s, buf);

            strcat(buf,"\t[OK]");
            sendto(udp_fd, buf, ret + 5, 0,
                        (struct sockaddr *)&src_addr, src_len);
            continue;
        }
    }

END:
    close (udp_fd);
}

int main()
{
    StartServer();
    return 0;
}
