#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sigaction.h>
#define _GNU_SOURCE
void timeout(int sig)
{
    if(sig == SIGALRM)
        printf("Time out !\n");
    alarm(2);
}

int main(int argc, char *argv[])
{
    int i;
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, 0);
    alarm(2);
    for(i = 0; i < 3; i++)
    {
        printf("wait...");
        sleep(100);
    }
    return 0;
}