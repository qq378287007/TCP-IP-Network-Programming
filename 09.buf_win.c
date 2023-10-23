#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void ShowSocketBufSize(SOCKET sock)
{
    int snd_buf;
    int len = sizeof(snd_buf);
    int state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&snd_buf, &len);
    if (state == SOCKET_ERROR)
        ErrorHanding("getsockopt() error");
    printf("Output buffer size: %d\n", snd_buf);

    int rcv_buf;
    len = sizeof(rcv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&rcv_buf, &len);
    if (state == SOCKET_ERROR)
        ErrorHanding("getsockopt() error");
    printf("Input buffer size: %d\n", rcv_buf);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);

    ShowSocketBufSize(sock);

    int snd_buf = 1024 * 3;
    int state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&snd_buf, sizeof(snd_buf));
    if (state == SOCKET_ERROR)
        ErrorHanding("setsockopt() error");

    int rcv_buf = 104 * 3;
    state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&rcv_buf, sizeof(rcv_buf));
    if (state == SOCKET_ERROR)
        ErrorHanding("setsockopt() error");

    ShorSocketBufSize(sock);

    closesocket(sock);

    WSACleanup();

    return 0;
}

// gcc 09.buf_win.c -o 09.buf_win -lws2_32 && 09.buf_win
