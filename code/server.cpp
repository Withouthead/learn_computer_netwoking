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
void sigchld_handler(int s)
{
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}
void *get_in_addr(sockaddr *sa)
{
    if(sa->sa_family == AF_INET)
        return &(((sockaddr_in *)sa)->sin_addr);
    return &(((sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
    int sockfd, new_fd;//欢迎socket 和 连接socket
    addrinfo hints, *servinfo, *p;//servinfo 是本地ip地址链
    sockaddr_storage their_addr; // 用来存储ipv4/6地址
    socklen_t sin_size;
    struct sigaction sa; //负责回收zombies processes ?
    int yes = 1; 
    char s[INET6_ADDRSTRLEN];//ip地址以字符串来表示
    int rv; //各个函数返回的值用它来接收

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; //表示ipv4/6都可
    hints.ai_socktype = SOCK_STREAM;//TCP
    hints.ai_flags = AI_PASSIVE;// 本地ip地址

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
    if(listen(sockfd, BACKLOG) == -1)//监听最多BACKLOG个访问
    {
        perror("listen");
        exit(1);
    }
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
    printf("server: waiting for connections...\n");
    while(1)
    {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if(new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
        if(!fork())
        {
            close(sockfd);
            if(send(new_fd, "Hello, world!", 13, 0) == -1)
            {
                perror("send");
            }
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }
    return 0;
};