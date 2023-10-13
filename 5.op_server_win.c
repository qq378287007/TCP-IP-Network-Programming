#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 9999
#define BUF_SIZE 1024
#define OPSZ 4

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int calculate(int opnum, int opnds[], char op)
{
    int result = opnds[0];
    int i;
    switch (op)
    {
    case '+':
        for (i = 1; i < opnum; i++)
            result += opnds[i];
        break;
    case '-':
        for (i = 1; i < opnum; i++)
            result -= opnds[i];
        break;
    case '*':
        for (i = 1; i < opnum; i++)
            result *= opnds[i];
        break;
    }
    return result;
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

    for (int i = 0; i < 5; i++)
    {
        SOCKADDR_IN clntAddr;
        SOCKET hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szAddr);
        // if(hClntSock==INVALID_SOCKET)
        if (hClntSock == -1)
            ErrorHanding("accept() error");
        else
            printf("Connected client %d\n", i + 1);

        int opnd_cnt = 0;
        //接收一个字节，数值个数
        recv(hClntSock, (char *)&opnd_cnt, 1, 0);

        char opinfo[BUF_SIZE];
        int recv_len = 0;
        //接收opnd_cnt个数值+1个运算符
        while (recv_len < OPSZ * opnd_cnt + 1)
        {
            int recv_cnt = recv(hClntSock, &opinfo[recv_len], BUF_SIZE - 1 - recv_len, 0);
            recv_len += recv_cnt;
        }

        int result = calculate(opnd_cnt, (int *)opinfo, opinfo[recv_len - 1]);
        send(hClntSock, (char *)&result, sizeof(result), 0);

        closesocket(hClntSock);
    }

    closesocket(hServSock);

    WSACleanup();

    return 0;
}