#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if (sig == SIGALRM)
        puts("Time out!");
    //alarm(2);
}

void keycontrol(int sig)
{
    if (sig == SIGINT)
        puts("CTRL+C pressed");
}

int main(int argc, char *argv[])
{
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);

    alarm(2);

    for (int i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(10); // 传输信号会唤醒进程，sleep剩余时间跳过
    }

    return 0;
}