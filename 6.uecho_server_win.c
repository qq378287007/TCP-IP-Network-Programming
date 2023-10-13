#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

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

    SOCKET serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == INVALID_SOCKET)
        ErrorHanding("socket() error");

    int addr_size = sizeof(SOCKADDR_IN);

    SOCKADDR_IN serv_addr;
    memset(&serv_addr, 0, addr_size);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (SOCKADDR *)&serv_addr, addr_size) == SOCKET_ERROR)
        ErrorHanding("bind() error");

    while (1)
    {
        char message[BUF_SIZE];
        SOCKADDR_IN clnt_addr;
        int str_len = recvfrom(serv_sock, message, BUF_SIZE-1, 0, (SOCKADDR *)&clnt_addr, &addr_size);
        sendto(serv_sock, message, str_len, 0, (SOCKADDR *)&clnt_addr, addr_size);
    }

    closesocket(serv_sock);

    WSACleanup();

    return 0;
}