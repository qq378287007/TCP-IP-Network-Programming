#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    char buf[BUF_SIZE];

    int fds[2];
    pipe(fds);

    pid_t pid = fork();
    if (pid == 0)
    {
        char str1[] = "Who are you?";
        write(fds[1], str1, sizeof(str1));
        sleep(2); // 防止管道内写入数据被本进程读取
        read(fds[0], buf, BUF_SIZE);
        printf("Child proc output: %s\n", buf);
    }
    else
    {
        char str2[] = "Thank you for your message";
        read(fds[0], buf, BUF_SIZE);
        printf("Parent proc output: %s\n", buf);
        write(fds[1], str2, sizeof(str2));
        sleep(3); // 防止父进程终止时，弹出命令提示符
    }

    return 0;
}

// gcc 11.pipe2.c -o 11.pipe2 && ./11.pipe2