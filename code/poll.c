#include <stdio.h>
#include <poll.h>
int main()
{
    struct pollfd pfds[1];
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;
    printf("Hit return or wait 2.5second for timeout\n");
    int num_events = poll(pfds, 1, 2500);
    printf("test");
    if (num_events == 0)
    {
       printf("Poll timed out !"); 
    }
    else
    {
        int pollin_happend = pfds[0].revents & POLLIN;
        if(pollin_happend)
        {
            printf("File descripter %d is ready to read\n", pfds->fd);
        }
        else
        {
            printf("Unexpected event occurred: %d\n", pfds[0].revents);
        }
    }
    return 0;
}