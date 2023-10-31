#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define PORT 9999
#define BUF_SIZE 100

void ErrorHanding(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void CompressSockets(SOCKET hSockArr[], int idx, int total)
{
    for (int i = idx; i < total; i++)
        hSockArr[i] = hSockArr[i + 1];
}

void CompressEvents(WSAEVENT hEventArr[], int idx, int total)
{
    for (int i = idx; i < total; i++)
        hEventArr[i] = hEventArr[i + 1];
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    SOCKET serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == INVALID_SOCKET)
        ErrorHanding("socket() error!");

    int opt = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
        ErrorHanding("setsockopt() error!");

    int addr_size = sizeof(SOCKADDR_IN);

    SOCKADDR_IN serv_addr;
    memset(&serv_addr, 0, addr_size);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (SOCKADDR *)&serv_addr, addr_size) == SOCKET_ERROR)
        ErrorHanding("bind() error!");

    if (listen(serv_sock, 5) == SOCKET_ERROR)
        ErrorHanding("listen() error!");

    WSAEVENT newEvent = WSACreateEvent();
    if (WSAEventSelect(serv_sock, newEvent, FD_ACCEPT) == SOCKET_ERROR) // 连接
        ErrorHanding("WSAEventSelect() error!");

    // 保存通过WSAEventSelect函数连接的套接字和事件对象
    SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
    int numOfClntSock = 0;
    hSockArr[numOfClntSock] = serv_sock;
    hEventArr[numOfClntSock] = newEvent;
    numOfClntSock++;

    while (1)
    {
        // 等待事件对象句柄hEventArr转化为signaled状态
        // 套接字集合中至少一个套接字发生了事件
        int posInfo = WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
        int startIdx = posInfo - WSA_WAIT_EVENT_0; // 最小起始索引

        // 遍历事件对象
        for (int i = startIdx; i < numOfClntSock; i++)
        {
            // 非阻塞获取事件对象状态
            int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);
            if (sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT)
            {
                continue;
            }
            else
            {
                // 获取事件类型
                sigEventIdx = i;
                WSANETWORKEVENTS netEvents;
                WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);

                // 判断发生的事件
                if (netEvents.lNetworkEvents & FD_ACCEPT)
                {
                    if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
                    {
                        puts("ACCEPT Error");
                        break;
                    }

                    // 新连接
                    SOCKADDR_IN clnt_addr;
                    SOCKET clnt_sock = accept(hSockArr[sigEventIdx], (SOCKADDR *)&clnt_addr, &addr_size);
                    newEvent = WSACreateEvent();
                    WSAEventSelect(clnt_sock, newEvent, FD_READ | FD_CLOSE); // 读取和关闭

                    hSockArr[numOfClntSock] = clnt_sock;
                    hEventArr[numOfClntSock] = newEvent;
                    numOfClntSock++;
                    puts("connected new client...");
                }
                if (netEvents.lNetworkEvents & FD_READ)
                {
                    if (netEvents.iErrorCode[FD_READ_BIT] != 0)
                    {
                        puts("READ Error");
                        break;
                    }

                    // 缓存区间读取数据，并发送
                    char msg[BUF_SIZE];
                    /*
                    int strLen = recv(hSockArr[sigEventIdx], msg, sizeof(msg), 0);
                    send(hSockArr[sigEventIdx], msg, strLen, 0);
                    */
                    int strLen;
                    do
                    {
                        strLen = recv(hSockArr[sigEventIdx], msg, sizeof(msg), 0);
                        if (strLen > 0)
                            send(hSockArr[sigEventIdx], msg, strLen, 0);
                    } while (strLen == BUF_SIZE);
                }
                if (netEvents.lNetworkEvents & FD_CLOSE)
                {
                    if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
                    {
                        puts("CLOSE Error");
                        break;
                    }

                    // 释放事件对象句柄，关闭套接字
                    WSACloseEvent(hEventArr[sigEventIdx]);
                    closesocket(hSockArr[sigEventIdx]);

                    numOfClntSock--;
                    CompressSockets(hSockArr, sigEventIdx, numOfClntSock);
                    CompressEvents(hEventArr, sigEventIdx, numOfClntSock);
                }
            }
        }
    }

    closesocket(serv_sock);

    WSACleanup();

    return 0;
}

// gcc 21.AsyncNotiEchoServ_win.c -o 21.AsyncNotiEchoServ_win -lws2_32 && 21.AsyncNotiEchoServ_win
