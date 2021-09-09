#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
int main()
{
    addrinfo hints, *res;
    int sockfd;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("www.example.com", "3490", &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
}