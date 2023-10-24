#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(0, &reads); // 0 is standard input(console)

    while (1)
    {
        fd_set temps = reads;
        struct timeval timeout = {5, 0};
        // 调用select后，timeout的tv_sec和tv_usec将被替换为超时前剩余时间
        int result = select(1, &temps, 0, 0, &timeout);
        if (result == -1)
        {
            puts("select() error!");
            break;
        }
        else if (result == 0)
        {
            puts("Time-out!");
            continue;
        }
        else
        {
            if (FD_ISSET(0, &temps))
            {
                char buf[BUF_SIZE];
                int str_len = read(0, buf, BUF_SIZE - 1);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }

    return 0;
}

// gcc 12.select_linux.c -o 12.select_linux && ./12.select_linux
