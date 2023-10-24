#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// 224.0.0.0 ~ 239.255.255.255
#define IP "224.1.1.2"
#define PORT 9999
#define TTL 64
#define BUF_SIZE 30

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

    SOCKET hSendSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (hSendSock == INVALID_SOCKET)
        ErrorHanding("socket() error");

    int opt = 1;
    if (setsockopt(hSendSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
        ErrorHanding("setsockopt() error");

    int time_live = TTL;
    if (setsockopt(hSendSock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&time_live, sizeof(time_live)) < 0)
        ErrorHanding("setsockopt() error");

    int addr_size = sizeof(SOCKADDR);

    SOCKADDR_IN mulAdr;
    memset(&mulAdr, 0, addr_size);
    mulAdr.sin_family = AF_INET;
    mulAdr.sin_addr.s_addr = inet_addr(IP); // Multicast IP
    mulAdr.sin_port = htons(PORT);          // Multicast Port

    FILE *fp;
    if ((fp = fopen("14.news_sender_mul_win.c", "r")) == NULL)
        ErrorHanding("fopen() error!");
    while (!feof(fp))
    {
        char buf[BUF_SIZE] = {0};
        fgets(buf, BUF_SIZE - 1, fp);
        sendto(hSendSock, buf, strlen(buf), 0, (SOCKADDR *)&mulAdr, addr_size);
        Sleep(200);
    }
    fclose(fp);

    closesocket(hSendSock);

    WSACleanup();

    return 0;
}

// gcc 14.news_sender_mul_win.c -o 14.news_sender_mul_win -lws2_32 && ./14.news_sender_mul_win
