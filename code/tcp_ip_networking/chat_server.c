#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define MAX_CLIENT 256

int client_socks[MAX_CLIENT];
int client_num = 0;

pthread_mutex_t mutex;
void error_handler(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(-1);
}

void send_message(char *message, int len)
{
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < client_num; i++)
    {
        write(client_socks[i], message, len);
    }
    pthread_mutex_unlock(&mutex);
}
void *handle_client(void *arg)
{
    int client_sock = *(int *)arg;
    int str_len = 0;
    char buf[BUF_SIZE];
    while((str_len=read(client_sock, buf, BUF_SIZE)))
    {
        send_message(buf, str_len);
    }
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < client_num; i++)
    {
        if(i == client_sock)
        {
            client_socks[i] = client_socks[client_num - 1];
            client_num --;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    close(client_sock);
    return NULL;
}
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        error_handler("Usage: chat_server <port>");
    }
    struct sockaddr_in server_addr, client_addr;
    int server_sock, client_sock;
    
    socklen_t addr_len;
    

    
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

    pthread_mutex_init(&mutex, NULL);
    while(1)
    {
        addr_len = sizeof(client_addr);
        printf("waitting connection\n");
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if(client_sock < 0)
        {
            continue;
        }
        printf("hi");
        pthread_mutex_lock(&mutex);
        printf("clifent_sock %d\n", client_sock);
        client_socks[client_num++] = client_sock;
        pthread_mutex_unlock(&mutex);

        pthread_t t_id;
        pthread_create(&t_id, NULL, handle_client, (void *)&client_sock);
        pthread_detach(t_id);
        printf("new connect has been set up, ip is %s\n", inet_ntoa(client_addr.sin_addr));

    }
    return 0;
}
