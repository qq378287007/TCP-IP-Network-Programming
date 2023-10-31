#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>

#define PORT 9999
#define BUF_SIZE 2048
#define BUF_SMALL 100

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

// 查找字符串str在buf中首次出现的位置
int GetLine(const char *buf, const char *str)
{
    int m = strlen(buf);
    int n = strlen(str);
    for (int i = 0; i <= m - n; i++)
        if (strncmp(buf + i, str, n) == 0)
            return i;
    return -1;
}

// 根据文件名的后缀返回对应的MIME类型
char *ContentType(char *file)
{
    char extension[BUF_SMALL] = {0};
    int n = GetLine(file, ".");
    strcpy(extension, file + n + 1);

    if (!strcmp(extension, "html") || !strcmp(extension, "htm"))
        return "text/html";
    else
        return "text/plain";
}

// 返回错误响应
void SendErrorMSG(SOCKET sock)
{
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char servName[] = "Server: simple web server\r\n";
    char cntType[] = "Content-Type: text/html\r\n\r\n";
    char content[] = "<html><head><title>NETWORK</title></head><body>"
                     "<font size=+5>error!<br/>filename and method!</font>"
                     "</body></html>";
    char cntLen[256];
    sprintf(cntLen, "Content-Length: %d\r\n", strlen(content));

    send(sock, protocol, strlen(protocol), 0);
    send(sock, servName, strlen(servName), 0);
    send(sock, cntLen, strlen(cntLen), 0);
    send(sock, cntType, strlen(cntType), 0);
    send(sock, content, strlen(content), 0);
}

// 读取文件内容，作为响应数据发送
void SendData(SOCKET sock, char *ct, char *fileName)
{
    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char servName[] = "Server: simple web server\r\n";
    char cntType[BUF_SMALL];
    sprintf(cntType, "Content-Type: %s\r\n\r\n", ct);

    FILE *fp;
    if ((fp = fopen(fileName, "r")) == NULL)
    {
        SendErrorMSG(sock);
        return;
    }
    fseek(fp, 0, SEEK_END);     // 将文件指针指向该文件的最后
    long file_size = ftell(fp); // 文件大小，大于等于文件字符数（多余字符为NULL）

    char *tmp = (char *)malloc(file_size);
    memset(tmp, '\0', file_size);
    fseek(fp, 0, SEEK_SET);                  // 重新将指针指向文件首部
    fread(tmp, sizeof(char), file_size, fp); // 开始读取整个文件
    fclose(fp);

    char cntLen[256];
    sprintf(cntLen, "Content-Length: %d\r\n", strlen(tmp));

    send(sock, protocol, strlen(protocol), 0);
    send(sock, servName, strlen(servName), 0);
    send(sock, cntLen, strlen(cntLen), 0);
    send(sock, cntType, strlen(cntType), 0);
    send(sock, tmp, strlen(tmp), 0);

    free(tmp);
}

unsigned WINAPI RequestHandler(void *arg)
{
    SOCKET hClntSock = *(SOCKET *)arg;
    int timeout = 100; // 0.1s
    int ret = setsockopt(hClntSock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

    char buf[BUF_SIZE] = {0};
    if (recv(hClntSock, buf, BUF_SIZE - 1, 0) <= 0) // 一次性读取所有请求内容
    {
        SendErrorMSG(hClntSock);
        closesocket(hClntSock);
        return 1;
    }
    // printf("recv:\n%s", buf);

    char line[256] = {0};
    int n = GetLine(buf, "\r\n");      // 实际上，只需要读取请求行
    strncpy(line, buf, n);             // 请求行
    if (strstr(line, "HTTP/") == NULL) // 协议
    {
        SendErrorMSG(hClntSock);
        closesocket(hClntSock);
        return 1;
    }

    char method[BUF_SMALL] = {0};
    n = GetLine(line, " /");
    strncpy(method, line, n);
    if (strcmp(method, "GET")) // 方法
    {
        SendErrorMSG(hClntSock);
        closesocket(hClntSock);
        return 1;
    }

    char fileName[BUF_SMALL] = {0};
    n = GetLine(line + 5, " ");
    if (n <= 0)
    {
        SendErrorMSG(hClntSock);
        closesocket(hClntSock);
        return 1;
    }
    strncpy(fileName, line + 5, n); // 请求文件

    char ct[BUF_SMALL] = {0};
    strcpy(ct, ContentType(fileName)); // 文件类型

    SendData(hClntSock, ct, fileName);
    closesocket(hClntSock);
    return 0;
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    SOCKET hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHanding("socket() error!");

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

    while (1)
    {
        SOCKADDR_IN clntAddr;
        SOCKET hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szAddr);
        if (hClntSock == INVALID_SOCKET)
            ErrorHanding("accept() error!");
        printf("Connection Request: %s:%d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));

        unsigned dwThreadID;
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, RequestHandler, (void *)&hClntSock, 0, &dwThreadID);
    }

    closesocket(hServSock);

    WSACleanup();

    return 0;
}

// gcc 24.webserv_win.c -o 24.webserv_win -lws2_32 && 24.webserv_win
// curl -v http://127.0.0.1:9999/24.webserv_win.c
