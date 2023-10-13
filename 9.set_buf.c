#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    int snd_buf = 1024 * 3;
    int state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, sizeof(snd_buf));
    if (state)
        error_handling("setsockopt() error");

    int rcv_buf = 104 * 3;
    state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, sizeof(rcv_buf));
    if (state)
        error_handling("setsockopt() error");

    socklen_t len = sizeof(snd_buf);

    //输出缓冲大小
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &len);
    if (state)
        error_handling("getsockopt() error");
    printf("Output buffer size: %d\n", snd_buf);

    //输入缓冲大小
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, &len);
    if (state)
        error_handling("getsockopt() error");
    printf("Input buffer size: %d\n", rcv_buf);

    close(sock);

    return 0;
}