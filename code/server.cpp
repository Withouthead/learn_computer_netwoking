#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"
#define BACKLOG 10
int main()
{
    int sockfd, new_fd;
    addrinfo hints, *servinfo, *p;
    sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) !=0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror);
        return 1;
    }
    for(p = servinfo; p != NULL; p = servinfo->ai_next)
    {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol) == -1))
        {
            perror("server: socket");
            continue;
        }
        if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1))
        {
            perror("setsockpot");//该端口被占用，所以直接退出程序
            exit(1);
        }
        if((bind(sockfd, p->ai_addr, p->ai_addrlen)== -1))
        {
            close(sockfd);
            perror("server: bind");//绑定端口失败
            continue;
        }
        break;//绑定第一个成功的ip即可

    }

    freeaddrinfo(servinfo);//释放地址链表内存
    if (p == NULL)
    {
        fprintf(stderr, "server; failed to bind\n");
        exit(1);
    }
    if(listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
}