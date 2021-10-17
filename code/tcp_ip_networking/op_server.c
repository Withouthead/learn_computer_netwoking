#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#define BUF_SIZE 1024
int main(int argc, char *argv[])
{
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    char message[BUF_SIZE];
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        return -1;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htol(INADDR_ANY);
    server_addr.sin_port = htons(argv[1]);

    if(bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        return -1;
    
    if(listen(sock, 5) < 0)
        return -1;
    
    for(int i = 0; i < 5; i ++)
    {
        int client_sock = accept(sock, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if(client_sock < 0)
            return -1;
        printf("connectint to %s\n", inet_ntoa(client_addr.sin_addr));
        int str_len = 0;
        while(str_len = read(client_sock, message, BUF_SIZE))
        {
            message[str_len] = 0;
            printf("receive message : %s\n", message);
        }
        write(client_sock, message, strlen(message));
        close(client_sock);
    }
    close(sock);
    return 0;
}