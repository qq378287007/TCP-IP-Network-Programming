#include <stdio.h>
#include <unistd.h>
#include <signal.h>

//信号处理函数，信号处理器
void timeout(int sig)
{
    if (sig == SIGALRM)
        puts("Time out!");
    // alarm(2);
}

void keycontrol(int sig)
{
    if (sig == SIGINT)
        puts("CTRL+C pressed");
}

int main(int argc, char *argv[])
{
    //注册SIGALRM、SIGINT信号及相应处理器
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    alarm(2);

    for (int i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(10); // 传输信号会唤醒进程（唤醒后不会再进入睡眠状态），sleep剩余时间跳过
    }

    return 0;
}

// gcc 10.signal.c -o 10.signal && ./10.signal
