#include <stdio.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
typedef SOCKET int;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
#define INVALID_SOCKET -1
#define closesocket close
#endif

#define PORT 9999

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main()
{

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");
#endif

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

    SOCKADDR_IN clntAddr;
    SOCKET hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szAddr);
    if (hClntSock == INVALID_SOCKET)
        ErrorHanding("accept() error");

    char message[] = "Hello World!";
    send(hClntSock, message, sizeof(message) - 1, 0);
    closesocket(hClntSock);

    closesocket(hServSock);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}