#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    char buf[BUF_SIZE];

    int fds1[2];
    pipe(fds1);

    int fds2[2];
    pipe(fds2);

    pid_t pid = fork();
    if (pid == 0)
    {
        char str1[] = "Who are you?";
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUF_SIZE);
        printf("Child proc output: %s\n", buf);
    }
    else
    {
        char str2[] = "Thank you for your message";
        read(fds1[0], buf, BUF_SIZE);
        printf("Parent proc output: %s\n", buf);
        write(fds2[1], str2, sizeof(str2));
        sleep(3); //防止父进程推出后弹出命令提示符
    }

    return 0;
}