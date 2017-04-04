
/*poll.h*/
/*int poll(struct pollfd *fds, unsigned int nfds, int timeout);*/

/*struct pollfd*/
/*{*/
    /*int fd;//轮询的文件描述符*/
    /*short events;//等待的事件*/
    /*short revents;//实际发生的事件*/

/*}*/

/*nfds  fds数组大小*/
/*timeout 超过时间毫秒，0立即返回，负数一直等待*/

/*超时返回0,出错返回-1,正常返回>0*/


/*POLLIN      有数据可读*/
/*POLLRDNORM  有普通数据可读*/
/*POLLRDBAND  有优先数据可读*/

/*POLLPRI     有紧迫数据可读*/

/*POLLOUT     可以写数据*/
/*POLLWRNORM  可以写普通数据*/
/*POLLWRBAND  可以写优先数据*/

/*POLLMSGSIGPOLL 消息可用*/

/*POLLER      发生错误   revents可用*/
/*POLLHUP     文件描述符挂起  revents可用*/

/*POLLNVAL    指定的文件描述符非法 revents可用*/


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <poll.h>

#include "../comm.h"

#define SER_PORT 0Xeeee
/*#define CLT_MAX  1000*/
#define CLT_MAX  5


typedef struct
{
    /*int fd;*/
    char ip[16];
    unsigned short port;
} CLT_S;

void Release(struct pollfd *pfds, size_t n)
{
    size_t i;

    for(i = 0; i < n; ++i)
    {
        close(pfds[i].fd);
    }

    free(pfds);

    exit(1);
}

void StartServer()
{
    struct pollfd *pfds;
    CLT_S clients[CLT_MAX];

    int tcp_fd, client_fd;// server, client
    int ret;
    struct sockaddr_in addr, src_addr; // server, client
    socklen_t len;// slength of ockaddr
    int i;
    int count_poll, count_tcp, begin;// begin = 2
    char buf[1024];// R/W with client
    int allowed;

    tcp_fd = socket(PF_INET, SOCK_STREAM, 0);//IPPROTO_IP

    if(tcp_fd < -1)
    {
        ELOG(socket failed);
        return;
    }

    //套路
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SER_PORT);

    len = sizeof(src_addr);

    // reuse addressess and binding
    allowed = 1;
    setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &allowed, sizeof(allowed));
    ret = bind(tcp_fd, (struct sockaddr *)&addr, len);

    if(ret == -1)
    {
        ELOG(bind failed);
        close(tcp_fd);
        return;
    }

    listen(tcp_fd, CLT_MAX);


    /*********************** poll **********************/
    pfds = (struct pollfd *)malloc((CLT_MAX + 2) * sizeof(struct pollfd));

    count_poll = 0;
    count_tcp = 0;

    //add stdin
    pfds[count_poll].fd = STDIN_FILENO;
    pfds[count_poll].events = POLLIN; // IN = POLLRDNORM | POLLRDBAND

    ++count_poll;

    //add tcp_fd
    pfds[count_poll].fd = tcp_fd;
    pfds[count_poll].events = POLLIN;

    ++count_poll;

    begin = count_poll;

    while(1)
    {
        //poll(3) infinity wait
        ret = poll(pfds, count_poll, -1);

        if(ret == -1)
        {
            ELOG(poll failed);
        }

        // first, check if there was an INPUT from STDIN_FILENO.
        if(pfds[0].revents & POLLIN /*!= 0*/)
        {
            LOG(INFO, read from bash...);

            /*memset(buf, 0, 1024);*/
            fgets(buf, 1024, stdin);
            buf[strlen(buf) - 1] = '\0'; // e.g.   abc\n -> abc\0

            if(strcmp(buf, "exit") == 0)
            {
                LOG(WRAN, server will exit...);
                break;
            }

            LOG(INFO, command not found.);
            continue;
        }

        // second, check if there was an INPUT from tcp_fd.
        if(pfds[1].revents & POLLIN/*!= 0*/)
        {
            client_fd = accept(tcp_fd, (struct sockaddr *)&src_addr, &len);

            if(client_fd == -1)
            {
                ELOG(accept failed);
                break;
            }
            else if(count_tcp >= CLT_MAX)
            {
                write(client_fd, "please wait...", 15);
                LOG(WRAN, connect over);
                close(client_fd);// do not forget
                continue;
            }
            // add client_fd
            clients[count_tcp].port = ntohs(src_addr.sin_port);
            strcpy(clients[count_tcp].ip, inet_ntoa(src_addr.sin_addr));
            ++count_tcp;

            // add pollfd
            pfds[count_poll].fd = client_fd;
            pfds[count_poll].events = POLLIN;
            ++count_poll;

            LOG(INFO, new connection IP:%s fd:%d,
                        clients[count_tcp - 1].ip,
                        client_fd);
            continue;
        }

        // handle fd of client
        for(i = begin/*, j = 0*/;
                   /*j < count_tcp ||*/i < count_poll;
                   ++i/*, ++j*/)
        {
            if(pfds[i].revents & POLLIN)
            {
                /*memset(buf, 0, sizeof(buf));*/
                ret = read(pfds[i].fd, buf, sizeof(buf));

                if(ret == -1)
                {
                    ELOG(read failed);
                    break;
                }
                else if(ret == 0)
                {
                    LOG(INFO, IP:%s:%d fd:%d has disconnected, 
                                clients[i - begin].ip, 
                                clients[i - begin].port,
                                pfds[i].fd);

                    close(pfds[i].fd);// do not forget.

                    /*i           2 3 4 5 6     2 3 4 5 6 -> 2 3 4 5 -*/
                    /*client fd   3 4 5 6 7  -> 3 4 7 6 7 -> 3 4 7 6 -*/
                    clients[i - begin] = clients[count_poll - 1 - begin];
                    pfds[i] = pfds[count_poll - 1];
                    --count_poll;
                    --count_tcp;

                    break;
                }

                buf[ret] = '\0';

                LOG(INFO, %s[fd:%d], buf, pfds[i].fd);

                strcat(buf, "\t[OK]");

                write(pfds[i].fd, buf, ret + 5);
            }
        }
    }

    Release(pfds, count_poll);
}


int main()
{
    StartServer();
    return 0;
}
