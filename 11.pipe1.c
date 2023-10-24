#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds[2];
    pipe(fds);

    pid_t pid = fork();
    if (pid == 0)
    {
        char str[] = "Who are you?";
        write(fds[1], str, sizeof(str));
    }
    else
    {
        char buf[BUF_SIZE];
        read(fds[0], buf, BUF_SIZE); // 阻塞
        puts(buf);
    }

    return 0;
}

// gcc 11.pipe1.c -o 11.pipe1 && ./11.pipe1
