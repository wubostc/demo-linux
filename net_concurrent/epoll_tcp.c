/*优点： select和poll的增强版本，linux下性能最好的并发模型*/
/*更加灵活，没有描述符限制*/
/*使用一个描述符管理多个描述符*/
/*监控描述符很多时，不会导致开销很大，不会在每次等待时把所有fd进行拷贝，只是在epoll_ctl时拷贝1次*/

/*#include <sys/epoll.h>*/
/*创建一个epoll句柄*/
/*epoll_create(1) 监听的文件描述符个数 size现在已经不起作用了*/
/*返回一个fd，用于管理所有要监控的文件描述符*/
/*int epoll_create(int size);*/

/*int  epoll_ctl(int epfd, int op, int fd, struct  epoll_event   *event)*/
/*注册 epoll 事件*/
/*epfd ： 管理的fd。  即epoll_create() 的返回值*/
/*op：    操作 EPOLL_CTL_[ ADD |  MOD | DEL ],   增改删*/
/*fd:        操作的 fd*/
/*event:   要监控的事件*/

/*struct epoll_event {*/
    /*_uint32_t         events;*/
    /*epoll_data_t     data;*/
/*}*/

/*typedef union epoll_data {*/
    /*void      *ptr;*/
    /*int          fd;*/
    /*uint32    u32;*/
    /*uint64    u64;  */
/*} epoll_data_t*/

/*监控事件 events*/
/*EPOLLIN                  fd可读*/
/*EPOLLOUT              fd可写*/
/*EPOLLPRI                fd紧急数据可读*/
/*EPOLLERR              fd发生错误*/
/*EPOLLHUP              fd 被挂起*/
/*EPOLLONESHOT    fd 只监控 1 次，监控完后自动删除*/
/*EPOLLET                 epoll 工作模式，设置为 边缘触发模式*/


/*int  epoll_wait(int epfd,  struct epoll_event *events,  int maxevents,  int timeout)*/
/*等待事件发生，类似select 调用*/
/*epfd            管理fd*/
/*events         等待发生的事件*/
/*maxevents    events 个数*/
/*timeout         超时时间(毫秒，0立即返回，-1一直等待)*/
/*返回值(超时返回0，正常返回待处理文件描述符个数，错误返回负数错误码)*/


#include "../comm.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/epoll.h>

#define SRV_PORT 0xeeee
/*#define MAX_CONN 100000*/
#define MAX_CONN 5


struct epoll_event events[MAX_CONN], ev;// ev用于注册事件，events来反射发生的事件

int clients[MAX_CONN];//all client fd
int count_conn;// epoll_wait
int count_fd;  // num of clients
void Release(int count)
{
    int i;

    for(i = 0; i < count; ++i)
    {
        close(clients[i]);
    }

}

void StartServer()
{
    char buf[1024];
    int epfd; // return value of epoll_create
    int tcp_fd; // server
    int ret;
    int fd; //client
    int i;

    struct sockaddr_in addr;
    struct sockaddr_in src_addr;
    socklen_t len;

    // 套路
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SRV_PORT);

    tcp_fd = socket(PF_INET, SOCK_STREAM, 0);

    if(tcp_fd == -1)
    {
        ELOG(socket failed);
        return;
    }


    int allowed = 1;
    setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &allowed, sizeof(allowed));

    len = sizeof(addr);
    ret = bind(tcp_fd, (struct sockaddr *)&addr, len);

    if(ret == -1)
    {
        ELOG(bind failed);
        close(tcp_fd);
        return;
    }

    listen(tcp_fd, MAX_CONN);

    /****************************************/
    /*Since Linux 2.6.8, the size argument is ignored, but must be  greater  than  zero*/
    epfd = epoll_create(MAX_CONN);

    if(epfd == -1)
    {
        ELOG(epoll_create failed);
        close(tcp_fd);
        return;
    }

    count_fd = 0;
    count_conn = 0;

    ev.data.fd= STDIN_FILENO;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    ++count_conn;

    ev.data.fd= tcp_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, tcp_fd, &ev);
    ++count_conn;

    while(1)
    {
        //epoll_wait(4) infinily wait
        //返回待处理数
        count_conn = epoll_wait(epfd, events, MAX_CONN, -1);//返回准备io操作的fd个数

        if(count_conn == -1)
        {
            ELOG(epoll_wait failed);
            usleep(1000); // 1 ms
            continue;
        }

        for(i = 0; i < count_conn; ++i) // multhread...
        {
            LOG(INFO, i:%d count_conn:%d, i, count_conn);

            if(!(events[i].events & EPOLLIN))
            {
                continue;
            }

            if(events[i].data.fd == STDIN_FILENO)
            {
                fgets(buf, 1024, stdin);
                buf[4] = 0;
                if(strcmp(buf, "exit") == 0)
                {
                    LOG(WRAN, server will exit...);
                    Release(count_fd);
                    exit(1);
                }

                LOG(INFO, command not found);

            }
            else if(events[i].data.fd == tcp_fd)
            {
                fd = accept(tcp_fd, (struct sockaddr *)&src_addr, &len);

                if(fd == -1)
                {
                    ELOG(accept failed);
                }

                if(count_fd == MAX_CONN)
                {
                    LOG(WRAN, connect over %d, count_conn);
                    write(fd, "please wait...", 15);
                    close(fd);
                }
                else
                {
                    clients[count_fd] = fd;
                    ++count_fd;
                    // add to fd of the epoll monitor
                    ev.data.fd = fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

                    ++count_conn;

                    LOG(INFO, new connection fd:%d count_conn:%d count_fd:%d, 
                                fd, count_conn, count_fd);
                }

            }
            else// client fd has input.
            {
                ret = read(events[i].data.fd, buf, sizeof(buf));

                if(ret == -1)
                {
                    ELOG(read failed);
                }
                else if(errno & (EINTR | EAGAIN))
                {
                    ELOG(error);
                    goto END;
                }
                else if(ret == 0)
                {
                    LOG(INFO, client: fd:%d has disconnected, events[i].data.fd);

                    ev = events[i];
                    // remove fd from epfd, no longer monitor this fd.
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    close(events[i].data.fd);
                    --count_conn;

                    // tcp_fd and STDIN_FILENO = 2
                    // count_fd+1 in accept
                    clients[i - 2] = clients[count_fd - 1];
                    --count_fd;
                }
                else
                {
                    buf[ret] = 0;

                    LOG(INFO, read:%s, buf);

                    strcat(buf, "\t[OK]");

                    write(events[i].data.fd, buf, ret + 5);

                    /*ev = events[i];*/
                    /*epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);*/
                }
            }
        }
    }

END:
    Release(count_fd);
}


int main()
{
    StartServer();

    return 0;
}
