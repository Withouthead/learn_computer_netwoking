#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>

#define NAME_SIZE 20
#define BUF_SIZE 1024

char name[NAME_SIZE];

void error_handler(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(-1);
}

void *send_msg(void *arg)
{
    int sock = *(int *)arg;
    char name_msg[NAME_SIZE + BUF_SIZE];
    char msg[BUF_SIZE];
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q") || !strcmp(msg, "Q"))
        {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s %s", name, msg);
        write(sock,name_msg, strlen(name_msg));
    }
    return NULL;
}

void *receive_msg(void *arg)
{
    int sock = *(int *)arg;
    char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    while(1)
    {
        str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        if(str_len < 0)
        {
            return NULL;
        }
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        error_handler("Usage: chat_client <ip> <port> <name>");
    }
    
    struct sockaddr_in server_addr;
    int client_sock;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock < 0)
    {
        error_handler("socket error");
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    sprintf(name, "[%s]", argv[3]);

    if(connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error_handler("connect err");
    }

    pthread_t send_thread, receive_thread;
    pthread_create(&send_thread, NULL, send_msg, (void *)&client_sock);
    pthread_create(&receive_thread, NULL, receive_msg, (void *)&client_sock);
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);
    close(client_sock);
    return 0;

}