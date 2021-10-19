#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sigaction.h>


#define BUF_SIZE 1024

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}
void err_handle(char *message)
{
    fprintf(stderr, "echo_mpserv err: %s\n", message);
}
int main(int argc, char *argv[])
{
    socklen_t addr_len;
    char buf[BUF_SIZE];
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htol(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    int state = sigaction(SIGCHLD, act, 0);

    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        err_handle("bind error");
        return -1;
    }
    if(listen(server_sock, 20) < 0)
    {
        err_handle("listen err");
        return -1;
    }
    while(1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if(client_sock < 0)
        {
            err_handle("accept err");
            continue;
        }
        else
            printf("new connect start...\n");
        pid_t pid = fork();
        if(pid < 0)
        {
            err_handle("fork err");
            close(client_sock);
            continue;
        }
        if(pid == 0)
        {
            close(server_sock);
            int str_index = 0;
            int str_len = 0;
            printf("recive message from %s by child process %d\n", inet_ntoa(client_addr.sin_addr), getpid());
            while ((str_len = read(client_sock, buf + str_len, BUF_SIZE)))
            {
                str_index += str_len;
            }
            buf[str_index] = 0;
            printf("%s\n", buf);
            write(client_sock, buf, str_index);
            close(client_sock);
            printf("message has send back!\n");
            return 0;
        }
        else
            close(client_sock);
    }
    close(server_sock);
    return 0;

}