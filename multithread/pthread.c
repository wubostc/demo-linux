/*
 是否熟悉POSIX多线程编程技术？如熟悉，编写程序完成如下功能：
  1）有一int型全局变量g_Flag初始值为0；
  2）在主线称中起动线程1，打印“this is thread1”，并将g_Flag设置为1
  3）在主线称中启动线程2，打印“this is thread2”，并将g_Flag设置为2
  4）线程序1需要在线程2退出后才能退出
  5）主线程在检测到g_Flag从1变为2，或者从2变为1的时候退出
   */

#include <pthread.h>
#include <stdio.h>

#define UNUSED(x) ((void *)(x))

int g_Flag;
pthread_mutex_t mtx;
pthread_cond_t cond;


void *foo(void *arg)
{
    UNUSED(arg);

    pthread_mutex_lock(&mtx);
    while(g_Flag == 0)
    {
        pthread_cond_wait(&cond,&mtx);
    }
    g_Flag = 1;
    fprintf(stdout,"this is thread1:%d\n",g_Flag);
    pthread_mutex_unlock(&mtx);

    return NULL;
}

void *bar(void *arg)
{
    UNUSED(arg);

    pthread_mutex_lock(&mtx);
    g_Flag = 2;
    fprintf(stdout,"this is thread2:%d\n",g_Flag);
    pthread_mutex_unlock(&mtx);
    pthread_cond_signal(&cond);

    return NULL;
}

int main()
{
    g_Flag = 0;
    pthread_mutex_init(&mtx,NULL);
    pthread_cond_init(&cond,NULL);

    pthread_t p1,p2;
    pthread_create(&p1,NULL,foo,NULL);
    pthread_create(&p2,NULL,bar,NULL);

    pthread_join(p1,NULL);
    pthread_join(p2,NULL);

    return 0;
}
