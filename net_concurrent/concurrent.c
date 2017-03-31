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
    int tcp_fd; // socket
    int ret;
    struct sockaddr_in addr;     // server
    struct sockaddr_in src_addr; // client
    socklen_t src_len;           // client
    int client_fd;
    int fds[MAX_CONN]; // manage the already connected fd
    int curr_conn;     // index of fds
    int max_fd;      // fds's limit
    int i;
    char buf[1024];  // used to R/W
    int allowed;     // allowed server to REUSEADDR


    // 0 STDIN, 1 STDOUT, 2 STDERR
    // tcp_fd 接下去一般为 3
    tcp_fd = socket(PF_INET, SOCK_STREAM, 0);

    if(tcp_fd == -1)
    {
        perror("socket failed");
        return;
    }

    allowed = 1;
    setsockopt(tcp_fd,
                SOL_SOCKET, SO_REUSEADDR, &allowed, sizeof(allowed));

    // 套路
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SER_PORT);

    ret = bind(tcp_fd, (struct sockaddr *)&addr, sizeof(addr));

    if(ret == -1)
    {
        perror("bind failed");
        close(tcp_fd);
        return;
    }

    ret = listen(tcp_fd, MAX_CONN);
    if(ret == -1)
    {
        perror("listen failed");
        close(tcp_fd);
        return;
    }

    fd_set fdset;
    max_fd = tcp_fd;

    src_len = sizeof(src_addr);

    curr_conn = 0;

    while(1)
    {
        FD_ZERO(&fdset);
        FD_SET(tcp_fd, &fdset);
        FD_SET(0, &fdset); // STDIN_FILENO

        for(i = 0; i < curr_conn; ++i)
        {
            FD_SET(fds[i], &fdset);

        }

        // select(1) 要监视的 fd 范围，[0, fd + 1)
        // select(2) 关心 fdset 读的变化
        // select(3) 关心 fdset 写的变化
        // select(4) 关心 fdset 错误的变化
        // select(5) 无限等待
        ret = select(max_fd + 1, &fdset, NULL, NULL, NULL);

        if(ret < 0)
        {
            perror("select failed");
            break;
        }
        else if(ret == 0)
        {
            //perror("timeout");
            //select 无限等待，不可能超时
            continue;
        }

        // tcp_fd 在 feset 中是否可读，表示是否有连接
        // 把连接的 fd 加入 feset 中，然后回到 select
        // 继续等待新的 fd 加入...
        if(FD_ISSET(tcp_fd, &fdset))
        {

            client_fd = accept(tcp_fd, (struct sockaddr *)&src_addr, &src_len);

            LOG(INFO, client_fd:%d, client_fd);

            if(client_fd == -1)
            {
                perror("accpet failed");
                break;
            }

            if(curr_conn == MAX_CONN) // MAX_CONN 5
            {
                LOG(WARN, connect over:%d, curr_conn);

                write(client_fd, "please wait...", 15);
                close(client_fd);
                continue;
            }

            LOG(INFO, client_fd %04d has been added!, client_fd);

            fds[curr_conn] = client_fd;
            FD_SET(fds[curr_conn], &fdset); // 加入 fdset 监视

            if(client_fd > max_fd)
            {
                max_fd = client_fd;
            }

            ++curr_conn;
            continue;
        }

        // 先检查是否有输入
        if(FD_ISSET(STDIN_FILENO, &fdset))
        {
            LOG(INFO,read from stdin...);

            ret = read(STDIN_FILENO, buf, 1024);

            buf[ret - 1] = '\0'; // remove '\n' in exit\n

            if(strcmp(buf, "exit") == 0)
            {
                goto END;
            }
            else
            {
                LOG(WARN, command not found!);
                continue;
            }
        }


        // 枚举除 tcp_fd 以外的可读 fd
        for(i = 0; i < curr_conn; ++i)
        {
            LOG(INFO, i:%d\tcurr_conn:%d, i, curr_conn);

            // fds[i] 是否处于可读状态
            if(FD_ISSET(fds[i], &fdset))
            {
                memset(buf, 0 , 1024);
                ret = read(fds[i], buf, 1024);

                LOG(INFO, ret:%d, ret);

                if(ret < 0)
                {
                    LOG(WARN, unkown error has occurred! client %d, fds[i]);
                    close(fds[i]);

                    break;
                }
                else if(ret == 0)
                {
                    LOG(INFO, client %d has been disconnected!, fds[i]);

                    close(fds[i]);

                    int j;
                    for(j = i; j < curr_conn; ++j)
                    {
                        fds[j] = fds[j + 1];
                    }
                    --curr_conn;

                    break;
                }

                // bytes > 0
                LOG(INFO, writing to client %d, fds[i]);

                buf[ret] = '\0';

                if(strcmp("kill", buf) == 0)
                {
                    LOG(WARN, server will exit...);
                    goto END;
                }

                printf("read[%d]:%s\n",fds[i], buf);
                strcat(buf, "\t\t[OK]");
                write(fds[i], buf, ret + 6); // add \t\t[OK]

                break;
            }
        }
    }

END:
    close (tcp_fd);
    for(i = 0; i < curr_conn; ++i)
    {
        close(fds[i]);
    }


}

int main()
{
    StartServer();
    return 0;
}
