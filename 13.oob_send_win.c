#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define IP "127.0.0.1"
#define PORT 9999
#define BUF_SIZE 30

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        ErrorHanding("socket() error!");

    int addr_size = sizeof(SOCKADDR_IN);

    SOCKADDR_IN addr;
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    if (connect(sock, (SOCKADDR *)&addr, addr_size) == SOCKET_ERROR)
        ErrorHanding("connect() error!");

    send(sock, "123", strlen("123"), 0);
    send(sock, "4", strlen("4"), MSG_OOB);

    send(sock, "567", strlen("567"), 0);
    send(sock, "890abc", strlen("890abc"), MSG_OOB); // MSG_OOB只发送最后一个字符c
    send(sock, "def", strlen("def"), 0);
    send(sock, "gh", strlen("gh"), MSG_OOB); // MSG_OOB只发送最后一个字符h

    closesocket(sock);

    WSACleanup();

    return 0;
}

// gcc 13.oob_send_win.c -o 13.oob_send_win -lws2_32 && 13.oob_send_win
