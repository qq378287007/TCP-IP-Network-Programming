#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define IP "127.0.0.1"
#define PORT 9999
#define BUF_SIZE 1024

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

    int szAddr = sizeof(SOCKADDR_IN);

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, szAddr);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(IP);
    servAddr.sin_port = htons(PORT);

    if (connect(hSock, (SOCKADDR *)&servAddr, szAddr) == SOCKET_ERROR)
        ErrorHanding("connect() error!");
    else
        puts("Connected.......");

    while (1)
    {
        fputs("Input message(Q to quit): ", stdout);
        char message[BUF_SIZE] = {0};
        fgets(message, BUF_SIZE - 1, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        int strLen = strlen(message);
        send(hSock, message, strLen, 0);
        int readLen = 0;
        while (1)
        {
            readLen += recv(hSock, &message[readLen], BUF_SIZE - 1, 0);
            if (readLen >= strLen)
                break;
        }
        message[strLen] = 0;
        printf("Message from server: %s\n", message);
    }

    closesocket(hSock);

    WSACleanup();

    return 0;
}

// gcc 23.StableEchoClnt_win.c -o 23.StableEchoClnt_win -lws2_32 && 23.StableEchoClnt_win
