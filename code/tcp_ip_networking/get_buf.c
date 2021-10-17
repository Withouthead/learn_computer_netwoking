#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
    int sock;
    int send_buf, recv_buf, state;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    socklen_t len = sizeof(send_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&send_buf, &len);
    
    len = sizeof(recv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&recv_buf, &len);
    printf("Input buffer size: %d \n", recv_buf);
    printf("Output buffer size: %d \n", send_buf);
    return 0;
}