#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>
#include <unistd.h>

#define IP "255.255.255.255"
#define PORT 9999
#define BUF_SIZE 30

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *sendSock(void *arg)
{
    SOCKET sock = *(SOCKET *)arg;

    int addr_size = sizeof(SOCKADDR_IN);

    SOCKADDR_IN brdAdr;
    memset(&brdAdr, 0, addr_size);
    brdAdr.sin_family = AF_INET;
    brdAdr.sin_addr.s_addr = inet_addr(IP); // Broad IP
    brdAdr.sin_port = htons(PORT);          // Broad Port

    /*
        if (connect(sock, (struct sockaddr *)&brdAdr, addr_size) == SOCKET_ERROR)
            ErrorHanding("connect() error");
    */

    sleep(1);

    FILE *fp;
    if ((fp = fopen("news_sender_brd_win.c", "r")) == NULL)
        ErrorHanding("fopen() error");

    while (!feof(fp))
    {
        char buf[BUF_SIZE] = {0};
        fgets(buf, BUF_SIZE - 1, fp);
        sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&brdAdr, addr_size);
        // write(sock, buf, strlen(buf));
        Sleep(200);
    }
    fclose(fp);

    closesocket(sock);
    return NULL;
}

void *recvSock(void *arg)
{
    SOCKET sock = *(SOCKET *)arg;
    while (1)
    {
        char buf[BUF_SIZE] = {0};
        int str_len = recvfrom(sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        //int str_len = read(sock, buf, BUF_SIZE - 1);
        if (str_len <= 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }
    return (void *)0;
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
        ErrorHanding("socket() error");

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
        ErrorHanding("setsockopt() error");

    int so_brd = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&so_brd, sizeof(so_brd)) < 0)
        ErrorHanding("setsockopt() error");

    int addr_size = sizeof(SOCKADDR_IN);
    SOCKADDR_IN localAdr;
    memset(&localAdr, 0, addr_size);
    localAdr.sin_family = AF_INET;
    localAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAdr.sin_port = htons(PORT);
    if (bind(sock, (struct sockaddr *)&localAdr, addr_size) == SOCKET_ERROR)
        ErrorHanding("bind() error");

    pthread_t t1;
    pthread_create(&t1, NULL, sendSock, (void *)&sock);
    pthread_t t2;
    pthread_create(&t2, NULL, recvSock, (void *)&sock);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    WSACleanup();

    return 0;
}