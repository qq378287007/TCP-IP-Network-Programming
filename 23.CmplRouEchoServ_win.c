#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

#define PORT 9999
#define BUF_SIZE 1024

typedef struct
{
    SOCKET socket;
    char buf[BUF_SIZE];//读写数据放置地址
    WSABUF wsaBuf;//放置读写的数据
} PER_IO_DATA, *LPPER_IO_DATA;

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

//回调函数，输入缓冲区读取结束后调用
void CALLBACK ReadCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET socket = hbInfo->socket;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD sentBytes;

    if (szRecvBytes == 0)//读取到EOF，客户端已断开连接
    {
        closesocket(socket);
        //是否malloc分配的内存
        free(lpOverlapped->hEvent);
        free(lpOverlapped);
        puts("Client disconnected.....");
    }
    else
    {
        //读取szRecvBytes字节成功，发送szRecvBytes字节
        bufInfo->len = szRecvBytes;
        //发送结束后调用WriteCompRoutine回调函数
        WSASend(socket, bufInfo, 1, &szRecvBytes, 0, lpOverlapped, WriteCompRoutine);
    }
}

//回调函数，输出缓冲区发送结束后调用
void CALLBACK WriteCompRoutine(DWORD dwError, DWORD szSendBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET socket = hbInfo->socket;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD recvBytes;
    DWORD flagInfo = 0;

	//发送完成后，读取输入缓冲区，最多读取BUF_SIZE字节
	//读取成功后，调用ReadCompRoutine函数
    bufInfo->len = BUF_SIZE;
    WSARecv(socket, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped, ReadCompRoutine);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");
        
	//WSA_FLAG_OVERLAPPED，重叠I/O套接字
    SOCKET hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hServSock == INVALID_SOCKET)
        ErrorHanding("WSAocket() error");

	//非阻塞套接字
    u_long mode = 1;
    ioctlsocket(hServSock, FIONBIO, &mode);

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

    while (1)
    {
    	//激活CR函数（重叠I/O可读写时调用）
        SleepEx(100, TRUE); // for alertable wait state，为了运行CR函数
        SOCKADDR_IN clntAddr;

        SOCKET hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szAddr);
        if (hClntSock == INVALID_SOCKET)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)//无客户端连接
                continue;
            else
                ErrorHanding("accept() error");
        }
        puts("Client connected.....");

        LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        hbInfo->socket = (DWORD)hClntSock;
        (hbInfo->wsaBuf).len = BUF_SIZE;
        (hbInfo->wsaBuf).buf = hbInfo->buf;

        LPWSAOVERLAPPED lpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
        memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));
        lpOvLp->hEvent = (HANDLE)hbInfo; // CR函数调用不需要事件对象，loPvLp->hEvent中可以写入其他信息

        DWORD recvBytes;
        DWORD flagInfo = 0;
        WSARecv(hClntSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOvLp, ReadCompRoutine);
    }

    closesocket(hServSock);

    WSACleanup();

    return 0;
}