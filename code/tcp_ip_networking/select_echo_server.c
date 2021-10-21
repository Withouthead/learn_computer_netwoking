#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SIZE 1024
void error_hanlder(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(-1);
}
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        error_hanlder("Usage: select_echo_server <port>");
    }

    socklen_t addr_len;
    fd_set readers, cp_readers;
    struct sockaddr_in server_addr, client_addr;
    int server_sock;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0)
    {
        error_hanlder("server_socket error!");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error_hanlder("bind error");
    }
    if(listen(server_sock, 15) < 0)
    {
        error_hanlder("listen error");
    }
    FD_ZERO(&readers);
    FD_SET(server_sock, &readers);
    int fd_max = server_sock;
    printf("init successfully\n");
    while(1)
    {
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        cp_readers = readers;
        int select_state = select(fd_max + 1, &cp_readers, 0, 0, &timeout);
        if(select_state < 0)
        {
            error_hanlder("select error");
        }
        if(select_state == 0)
        {
            printf("waiting to new connect\n");
        }
        if(select_state > 0)
        {
            printf("select_state has changed\n");
            for(int i = 0; i <= fd_max; i++)
            {
                if(!FD_ISSET(i, &cp_readers))
                {
                    continue;
                }
                printf("connect request!\n");
                if(i == server_sock)
                {
                    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
                    if(client_sock < 0)
                    {
                        continue;
                    }
                    printf("new connect has been set up, ip is %s\n", inet_ntoa(client_addr.sin_addr));
                    FD_SET(client_sock, &readers);
                    printf("old fd_max :%d\n", fd_max);
                    fd_max = fd_max > client_sock ? fd_max : client_sock;
                    printf("new fd_max :%d\n", fd_max);
                }
                else
                {
                    printf("receive message:\n");
                    int str_len = 0;
                    int str_size = 0;
                    char buf[BUF_SIZE];
                    while(str_len = read(i, buf+str_size, BUF_SIZE))
                    {
                        str_size += str_len;
                    }
                    buf[str_size] = 0;
                    printf("%s\n........\n", buf);
                    write(i, buf, strlen(buf));
                    printf("send message back successfully\n");
                    close(i);
                    FD_CLR(i, &readers);
                }
            }
        }
        
        
    }
    return 0;

}