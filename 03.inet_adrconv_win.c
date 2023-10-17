#include <stdio.h>
#include <string.h>
#include <winsock2.h>

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

    // inet_addr
    {
        char *addr = "127.212.124.78";
        unsigned long conv_addr = inet_addr(addr);
        if (conv_addr == INADDR_NONE)
            printf("Error occured!\n");
        else
            printf("Network ordered integer addr: %#lx\n", conv_addr);
    }

    // inet_ntoa
    {
        struct sockaddr_in addr;
        addr.sin_addr.s_addr = htonl(0x01020304);
        char *strPtr = inet_ntoa(addr.sin_addr);
        char strArr[20];
        strcpy(strArr, strPtr);
        printf("Dotted-Decimal notation: %s\n", strArr);
    }

    WSACleanup();

    return 0;
}

// gcc 03.inet_adrconv_win.c -o 03.inet_adrconv_win -lws2_32 && 03.inet_adrconv_win
