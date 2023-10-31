#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

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

    SOCKET hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hServSock == INVALID_SOCKET)
        ErrorHanding("WSAocket() error!");

    int opt = 1;
    if (setsockopt(hServSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
        ErrorHanding("setsockopt() error!");

    int szAddr = sizeof(SOCKADDR_IN);

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, szAddr);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT);

    if (bind(hServSock, (SOCKADDR *)&servAddr, szAddr) == SOCKET_ERROR)
        ErrorHanding("bind() error!");

    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHanding("listen() error!");

    SOCKADDR_IN clntAddr;
    SOCKET hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szAddr);
    if (hClntSock == INVALID_SOCKET)
        ErrorHanding("accept() error!");

    WSAEVENT evObj = WSACreateEvent();
    WSAOVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = evObj;

    char buf[BUF_SIZE] = {0};
    WSABUF dataBuf = {BUF_SIZE, buf};

    DWORD recvBytes = 0;
    DWORD flags = 0;
    if (WSARecv(hClntSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSA_IO_PENDING) // recv尚未完成（pending）
        {
            puts("Background data receive");
            // 等待接收完成
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
            // 获取接收字节数
            WSAGetOverlappedResult(hClntSock, &overlapped, &recvBytes, FALSE, NULL);
        }
        else
        {
            ErrorHanding("WSARecv() error!");
        }
    }

    printf("Received message: %s\n", buf);

    WSACloseEvent(evObj);

    closesocket(hClntSock);

    closesocket(hServSock);

    WSACleanup();

    return 0;
}

// gcc 22.OverlappedRecv_win.c -o 22.OverlappedRecv_win -lws2_32 && 22.OverlappedRecv_win
