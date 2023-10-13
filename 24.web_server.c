#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 9999
#define BUF_SIZE 2048
#define BUF_SMALL 100

int GetLine(const char *buf, const char *str)
{
    int m = strlen(buf);
    int n = strlen(str);
    for (int i = 0; i <= m - n; i++)
        if (strncmp(buf + i, str, n) == 0)
            return i;
    return -1;
}

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void SendErrorMSG(FILE *sock)
{
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char servName[] = "Server: simple web server\r\n";
    char cntType[] = "Content-Type: text/html\r\n\r\n";
    char content[] = "<html><head><title>NETWORK</title></head><body>"
                     "<font size=+5>error!<br>filename and method!</font>"
                     "</body></html>";
    char cntLen[256];
    sprintf(cntLen, "Content-Length: %ld\r\n", strlen(content));

    fputs(protocol, sock);
    fputs(servName, sock);
    fputs(cntLen, sock);
    fputs(cntType, sock);
    fputs(content, sock);

    fflush(sock);
}

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

void SendData(FILE *sock, char *ct, char *fileName)
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
    fseek(fp, 0, SEEK_END);     //将文件指针指向该文件的最后
    long file_size = ftell(fp); //文件大小，大于等于文件字符数（多余字符为NULL）

    char *tmp = (char *)malloc(file_size);
    memset(tmp, '\0', file_size);
    fseek(fp, 0, SEEK_SET);                  //重新将指针指向文件首部
    fread(tmp, sizeof(char), file_size, fp); //开始读取整个文件
    fclose(fp);

    char cntLen[256];
    sprintf(cntLen, "Content-Length: %ld\r\n", strlen(tmp));

    fputs(protocol, sock);
    fputs(servName, sock);
    fputs(cntLen, sock);
    fputs(cntType, sock);
    // send(sock, tmp, strlen(tmp), 0);
    fwrite(tmp, 1, strlen(tmp), sock);
    fflush(sock);

    free(tmp);
}

void *RequestHandler(void *arg)
{
    int hClntSock = *(int *)arg;
    int timeout = 100; // 0.1s
    int ret = setsockopt(hClntSock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

    FILE *clnt_read = fdopen(hClntSock, "r");
    FILE *clnt_write = fdopen(dup(hClntSock), "w");

    char buf[BUF_SIZE] = {0};
    if (fgets(buf, BUF_SIZE - 1, clnt_read) == NULL)
    {
        SendErrorMSG(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return (void *)1;
    }
    char line[256] = {0};
    int n = GetLine(buf, "\r\n");
    strncpy(line, buf, n);
    if (strstr(line, "HTTP/") == NULL)
    {
        SendErrorMSG(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return (void *)1;
    }

    char method[BUF_SMALL] = {0};
    n = GetLine(line, " /");
    strncpy(method, line, n);
    if (strcmp(method, "GET"))
    {
        SendErrorMSG(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return (void *)1;
    }

    char fileName[BUF_SMALL] = {0};
    n = GetLine(line + 5, " ");
    if (n <= 0)
    {
        SendErrorMSG(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return (void *)1;
    }
    strncpy(fileName, line + 5, n);

    char ct[BUF_SMALL] = {0};
    strcpy(ct, ContentType(fileName));

    SendData(clnt_write, ct, fileName);

    fclose(clnt_read);
    fclose(clnt_write);
    return NULL;
}

int main(int argc, char *argv[])
{
    int hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == -1)
        error_handling("socket() error");

    int opt = 1;
    if (setsockopt(hServSock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error");

    socklen_t szAddr = sizeof(struct sockaddr_in);

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, szAddr);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT);

    if (bind(hServSock, (struct sockaddr *)&servAddr, szAddr) == -1)
        error_handling("bind() error");

    if (listen(hServSock, 5) == -1)
        error_handling("listen() error");

    while (1)
    {
        struct sockaddr_in clntAddr;
        int hClntSock = accept(hServSock, (struct sockaddr *)&clntAddr, &szAddr);
        if (hClntSock == -1)
            error_handling("accept() error");

        printf("Connection Request: %s:%d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));

        pthread_t hThread;
        pthread_create(&hThread, NULL, RequestHandler, &hClntSock);
        pthread_detach(hThread);
    }

    close(hServSock);

    return 0;
}