#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if (sig == SIGALRM)
        puts("Time out!");
    alarm(2);
}

void keycontrol(int sig)
{
    if (sig == SIGINT)
        puts("CTRL+C pressed");
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    act.sa_handler = timeout;
    sigaction(SIGALRM, &act, 0);

    act.sa_handler = keycontrol;
    sigaction(SIGINT, &act, 0);

    alarm(2);

    for (int i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(100); // SIGALRM信号会唤醒进程
    }

    return 0;
}

// gcc 10.sigaction.c -o 10.sigaction && ./10.sigaction
