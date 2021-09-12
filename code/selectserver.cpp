#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"
void *get_in_addr(sockaddr *sa)
{
    if(sa->sa_family == AF_INET)
    {
        return &((sockaddr_in *)sa)->sin_addr;
    }
    return &((sockaddr_in6 *)sa)->sin6_addr;
}
int get_listener_socket()
{
    addrinfo hints, *servinfo, *p;
    int listener;
    int rv;
    int yes = 1;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) < 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    for(p = servinfo; p != NULL; p = servinfo->ai_next)
    {
        if((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            continue;
        }
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
        if(bind(listener, p->ai_addr, p->ai_addrlen))
        {
            close(listener);
        }
        break;
    }

    if(p == NULL)
    {
        fprintf(stderr, "selectserver: failed to bind\n");
        return -1;
    }
    freeaddrinfo(servinfo);
    if(listen(listener, 10) == -1)
    {
        perror("listen");
        return -1;
    }

    return listener;

}
int main()
{
    setvbuf (stdout, NULL, _IONBF, 0);
    fd_set set_master;
    fd_set read_fds;
    int fdmax = 0;
    int listener;
    int newfd;
    sockaddr_storage remoteaddr;
    socklen_t addrlen;
    char buf[256];
    int nbytes;
    char remoteIP[INET6_ADDRSTRLEN];
    int yes = 1;
    int rv;
    listener = get_listener_socket();
    if(listener == -1)
    {
        fprintf(stderr, "error getting listeing socket\n");
        exit(1);
    }
    FD_SET(listener, &set_master);
    fdmax = listener;
    while(true)
    {
        read_fds = set_master;
        if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(1);
        }
        for(int i = 0; i <= fdmax; i++)
        {
            if(FD_ISSET(i, &read_fds))
            {
                if(i == listener)
                {
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (sockaddr *)&remoteaddr, &addrlen);
                    if(newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        FD_SET(newfd, &set_master);
                        if(newfd > fdmax)
                            fdmax = newfd;
                        printf("selectserver: new connect from %s on socket %d", inet_ntop(remoteaddr.ss_family, get_in_addr((sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
                        send(newfd, "Hello!", 5, 0);
                    }
                }
                else
                {
                    if((nbytes == recv(i , buf, sizeof buf ,0)) <=0 )
                    {
                        if(nbytes == 0)
                            printf("selectsever:sock %d hung up\n", i);
                        else
                            printf("recv");
                        close(i);
                        FD_CLR(i, &set_master);
                    }
                    else
                    {
                        sockaddr addr;
                        socklen_t addrlen = sizeof addr;
                        getpeername(i, &addr, &addrlen);
                        printf("get message from %s on %d\n", (char *)get_in_addr(&addr), i);
                        printf("\"%s\"\n", buf);
                    }
                }
            }
        }
    }
    return 0;
}