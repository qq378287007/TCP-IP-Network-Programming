#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define IP "64.233.189.104"

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

    SOCKADDR_IN addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(IP);

    struct hostent *host = gethostbyaddr((char *)&addr.sin_addr, 4, AF_INET);
    if (!host)
        ErrorHanding("gethostbyaddr... error");

    printf("Official name: %s\n", host->h_name);

    for (int i = 0; host->h_aliases[i]; i++)
        printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);

    printf("Address type: %s\n",
           host->h_addrtype == AF_INET ? "AF_INET" : "AF_INET6");

    for (int i = 0; host->h_addr_list[i]; i++)
        printf("IP addr %d: %s\n", i + 1,
               inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));

    WSACleanup();

    return 0;
}

// gcc 08.gethostbyaddr_win.c -o 08.gethostbyaddr_win -lws2_32 && 08.gethostbyaddr_win
