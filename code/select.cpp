#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#define STDIN 0
int main()
{
    timeval tv;
    fd_set readfds;
    tv.tv_sec = 2;
    tv.tv_usec = 500000;
    FD_ZERO(&readfds);
    FD_SET(STDIN+1, &readfds);
    select(STDIN+1, &readfds, NULL, NULL, &tv);
    if(FD_ISSET(STDIN, &readfds))
    {
        printf("A key was pressed\n");
    }
    else
        printf("Timed out\n");
    return 0;
    

}