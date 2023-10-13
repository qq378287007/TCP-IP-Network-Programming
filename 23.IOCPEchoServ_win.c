#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

#define PORT 9999
#define BUF_SIZE 100
#define READ 3
#define WRITE 5

// socket info
typedef struct
{
    SOCKET hClntSock;
    SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// buffer info
typedef struct
{
    OVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[BUF_SIZE];
    int rwMode; // READ or WRITE
} PER_IO_DATA, *LPPER_IO_DATA;

unsigned WINAPI EchoThreadMain(LPVOID pComPort)
{
    while (1)
    {
        HANDLE hComPort = (HANDLE)pComPort;
        DWORD bytesTrans;
        LPPER_HANDLE_DATA handleInfo;
        LPPER_IO_DATA ioInfo;
        //阻塞，直到I/O数据读写执行完后往下执行
        // INFINITE，GetQueuedCompletionstatus函数在I/O完成且已注册相关信息时返回
        // &ioInfo == ioInfo->overlapped，结构体变量地址 == 结构体首成员地址
        GetQueuedCompletionStatus(hComPort, &bytesTrans, (PULONG_PTR)&handleInfo, (LPOVERLAPPED *)&ioInfo, INFINITE);

        SOCKET socket = handleInfo->hClntSock;

        if (ioInfo->rwMode == READ)
        {
            puts("message received!");
            if (bytesTrans == 0)
            {
                puts("close connect!");
                closesocket(socket);
                free(handleInfo);//客户端关闭时，释放空间
                free(ioInfo);
                continue;
            }

            //将I/O读取的数据发送回客户端
            memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
            ioInfo->wsaBuf.len = bytesTrans;
            ioInfo->rwMode = WRITE;
            WSASend(socket, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);

            //ioInfo必须malloc（区分send和recv），WSASend或WSARecv执行完后，都会调用GetQueuedCompletionStatus。
            //每一次send或recv后都会放入队列中，等待I/O完成后由GetQueuedCompletionStatus执行。
            ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
            memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
            ioInfo->wsaBuf.len = BUF_SIZE;
            ioInfo->wsaBuf.buf = ioInfo->buffer;
            ioInfo->rwMode = READ;

            DWORD flags = 0;
            WSARecv(socket, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
        }
        else
        {
            puts("message sent!");
            free(ioInfo);//释放
        }
    }
    return 0;
}

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

    HANDLE hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    //创建多个线程，且将CP对象句柄分配到线程
    for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++)
        _beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);

    SOCKET hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hServSock == INVALID_SOCKET)
        ErrorHanding("WSAocket() error");

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
        SOCKADDR_IN clntAddr;
        SOCKET hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szAddr);
        if (hClntSock == INVALID_SOCKET)
            ErrorHanding("accept() error");

        puts("new connect!");

        LPPER_HANDLE_DATA handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
        handleInfo->hClntSock = hClntSock;
        memcpy(&(handleInfo->clntAdr), &clntAddr, szAddr);

        //针对hClntSock套接字的重叠I/O完成时，已完成信息将写入连接的CP对象，这会引起GetQueue...函数的返回
        //关联客户端套接字，CP端口，句柄信息（区分不同客户端），I/O可读写时，GetQueue...会向下执行（类似条件变量）
        CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (ULONG_PTR)handleInfo, 0);

        LPPER_IO_DATA ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
        ioInfo->wsaBuf.len = BUF_SIZE;
        ioInfo->wsaBuf.buf = ioInfo->buffer;
        ioInfo->rwMode = READ; // IOCP不会区分输入完成和输出完成的状态（只通知完成I/O的状态），额外变量rwMode用于区分。

        //接收数据后，保存在ioInfo中
        DWORD recvBytes;
        DWORD flags = 0;
        WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);
    }

    closesocket(hServSock);

    CloseHandle(hComPort);

    WSACleanup();

    return 0;
}