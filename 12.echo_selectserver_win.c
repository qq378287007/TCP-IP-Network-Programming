#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    SOCKET hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHanding("socket() error");

    int opt = 1;
    if (setsockopt(hServSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
        ErrorHanding("setsockopt() error");

    int szAddr = sizeof(SOCKADDR_IN);

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, szAddr);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT);

    if (bind(hServSock, (SOCKADDR *)&servAddr, szAddr) == SOCKET_ERROR)
        ErrorHanding("bind() error");

    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHanding("listen() error");

    fd_set reads; // 记录所有文件描述符（套接字）
    FD_ZERO(&reads);
    FD_SET(hServSock, &reads); // 加入服务端套接字
    while (1)
    {
        fd_set cpyReads = reads; // 记录可读文件描述符（套接字）
        TIMEVAL timeout = {5, 5000};
        int fdNum = select(0, &cpyReads, 0, 0, &timeout);
        if (fdNum == SOCKET_ERROR)
            break;
        else if (fdNum == 0)
            continue;

        SOCKET hClntSock;
        for (int i = 0; i < reads.fd_count; i++) // 遍历所有文件描述符
        {
            if (FD_ISSET(reads.fd_array[i], &cpyReads)) // 当前文件描述符位于cpyReads集合中，可读
            {
                if (reads.fd_array[i] == hServSock) // 服务端套接字可读，表示有新的客户端连接
                {
                    SOCKADDR_IN clntAddr;
                    hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szAddr); // 新客户端套接字
                    FD_SET(hClntSock, &reads);                                     // 加入客户端套接字
                    printf("connected client: %d\n", hClntSock);
                }
                else // 客户端套接字可读
                {
                    char buf[BUF_SIZE];
                    int strLen = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0); // 从客户端套接字中读取缓冲区
                    if (strLen == 0)
                    {
                        closesocket(reads.fd_array[i]); // 关闭客户端套接字
                        printf("closed client: %d\n", reads.fd_array[i]);
                        FD_CLR(reads.fd_array[i], &reads); // 移除客户端套接字
                    }
                    else
                    {
                        send(reads.fd_array[i], buf, strLen, 0);
                    }
                }
            }
        }
    }

    closesocket(hServSock);

    WSACleanup();

    return 0;
}

// gcc 12.echo_selectserver_win.c -o 12.echo_selectserver_win -lws2_32 && 12.echo_selectserver_win
