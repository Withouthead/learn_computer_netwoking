#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    fd_set readset, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;
    FD_ZERO(&readset);
    FD_SET(0, &readset);
    while(1)
    {
        temps = readset;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        result = select(1, &temps, 0, 0, &timeout);
        if(result == -1)
        {
            printf("select() error!\n");
            return -1;
        }
        if(result == 0)
        {
            printf("Time out !\n");

        }
        else
        {
            if(FD_ISSET(0, &temps))
            {
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("message from console :%s\n", buf);
            }
        }
    }
    return  0;
}