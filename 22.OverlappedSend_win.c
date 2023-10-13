#include <stdio.h>
#include <stdlib.h>
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

    SOCKET hSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hSock == INVALID_SOCKET)
        ErrorHanding("WSASocket() error");

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(SOCKADDR_IN));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(IP);
    servAddr.sin_port = htons(PORT);

    if (connect(hSock, (SOCKADDR *)&servAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
        ErrorHanding("connect() error");

    WSAEVENT evObj = WSACreateEvent();

    WSAOVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = evObj;

    char msg[] = "Network is computer!";
    WSABUF dataBuf = {strlen(msg), msg};

    DWORD sendBytes = 0;
    if (WSASend(hSock, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSA_IO_PENDING) //send尚未完成（pending）
        {
            puts("Background data send");
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
            WSAGetOverlappedResult(hSock, &overlapped, &sendBytes, FALSE, NULL);
        }
        else
        {
            ErrorHanding("WSASend() error");
        }
    }

    printf("Send data size: %d\n", sendBytes);

    WSACloseEvent(evObj);

    closesocket(hSock);

    WSACleanup();

    return 0;
}