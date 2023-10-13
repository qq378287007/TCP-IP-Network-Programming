#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

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

    SOCKET hRecvSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (hRecvSock == INVALID_SOCKET)
        ErrorHanding("socket() error");

    int opt = 1;
    if (setsockopt(hRecvSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
        ErrorHanding("setsockopt() error");

    int addr_size = sizeof(SOCKADDR_IN);
    SOCKADDR_IN adr;
    memset(&adr, 0, addr_size);
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(PORT);
    if (bind(hRecvSock, (struct sockaddr *)&adr, addr_size) == SOCKET_ERROR)
        ErrorHanding("bind() error");

    while (1)
    {
        char buf[BUF_SIZE];
        int str_len = recvfrom(hRecvSock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len <= 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }

    closesocket(hRecvSock);

    WSACleanup();

    return 0;
}