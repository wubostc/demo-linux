#include <stdio.h>
#include <netdb.h>//struct hostent, gethostbyname
#include <arpa/inet.h>//inet_ntoa
/*#include <netinet/in.h>//struct in_addr*/
#include <stdlib.h>

int main(int argc,char *argv[])
{
    struct hostent *host;

    if(argc != 2)
    {
        fprintf(stderr,"usage:one baidu.com\n");
        exit(1);
    }

    if((host = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr,"gethostbyname failed.\n");
        exit(1);
    }

    //h_addr_list 存放网络序
    fprintf(stdout,"%s\n", inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));

    return 0;
}
