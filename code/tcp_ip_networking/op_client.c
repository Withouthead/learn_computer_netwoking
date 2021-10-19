#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handler(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(-1);
}
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "%s <ip> <ports>\n", argv[0]); 
        return -1;
    }
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        return -1;
    }
    char message[BUF_SIZE];
    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error_handler("can't connect remote server");
    }
    while(1)
    {
        printf("Input message (Q or q to quit)");
        scanf("%s", message);
        if(strcmp(message, "q") == 0 || strcmp(message, "Q") == 0)
        {
            break;
        }
        int str_len = write(sock, message, strlen(message));
        int recv_len = 0;
        while(recv_len < str_len)
        {
            int recv_cnt = read(sock, &message[recv_len], BUF_SIZE - 1);
            if(recv_cnt < 0)
            {
                error_handler("can't read from remote server");
            }
            recv_len += recv_cnt;
        }
        message[recv_len] = 0;
        printf("Message from server: %s", message);
    }
    close(sock);
    return 0;
}