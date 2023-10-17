#include <stdio.h>
#include <winsock2.h>

#define IP "127.0.0.1"
#define PORT 9999

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

    SOCKET hSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hSock == INVALID_SOCKET)
        ErrorHanding("socket() error!");

    const int szAddr = sizeof(SOCKADDR_IN);

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, szAddr);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(IP);
    servAddr.sin_port = htons(PORT);

    if (connect(hSock, (SOCKADDR *)&servAddr, szAddr) == SOCKET_ERROR)
        ErrorHanding("connect() error!");

    int strLen = 0;

    int readLen;
    char message[30];
    int idx = 0; 
    for (; readLen = recv(hSock, &message[idx], 1, 0); idx++)
    {
        if (readLen == -1)
            ErrorHanding("recv() error!");
        strLen += readLen;
    }
    message[strLen] = '\0';
    printf("Message from server: %s\n", message);
    printf("Function recv call count: %d\n", idx);

    closesocket(hSock);

    WSACleanup();

    return 0;
}

// gcc 02.tcp_client_win.c -o 02.tcp_client_win -lws2_32 && 02.tcp_client_win
