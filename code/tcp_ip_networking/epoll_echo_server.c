#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 4
#define EPOLL_SIZE 50
void error_handler(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(-1);
}
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        error_handler("Usage: epoll_echo_server <port>");
    }
    struct sockaddr_in server_addr, client_addr;
    int server_sock, client_sock;
    socklen_t addr_len;
    struct epoll_event *ep_events;
    struct epoll_event event;
    

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0)
    {
        error_handler("socket error");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error_handler("bind error");
    }
    
    if(listen(server_sock, 15) < 0)
    {
        error_handler("listen error");
    }

    int epfd = epoll_create(EPOLL_SIZE);
    event.events = EPOLLIN;
    event.data.fd = server_sock;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &event) < 0)
    {
        error_handler("epoll_ctl erroe");
    }
    ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    while(1)
    {
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        for(int i = 0 ; i < event_cnt; i++)
        {
            if(ep_events[i].data.fd == server_sock)
            {
                client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
                if(client_sock < 0)
                {
                    continue;
                }
                event.events = EPOLLIN|EPOLLET;
                event.data.fd = client_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event);
                printf("new connect has been set up, ip is %s\n", inet_ntoa(client_addr.sin_addr));

            }
            else
            {
                char buf[BUF_SIZE];
                getpeername(ep_events[i].data.fd, (struct sockaddr *)&client_addr, &addr_len);
                int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                if(str_len == 0)
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);
                    printf("close ip %s client: %d \n", inet_ntoa(client_addr.sin_addr), ep_events[i].data.fd);
                }

                else
                {
                    buf[str_len] = 0;
                    printf("receive data from %s : %s\n", inet_ntoa(client_addr.sin_addr), buf);
                    write(ep_events[i].data.fd, buf, strlen(buf));
                }
            }
        }
    }

    return 0;
}
