#include <stdio.h>
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

    unsigned short host_port = 0x1234;
    unsigned short net_port = htons(host_port);
    printf("Host ordered port: %#x\n", host_port);
    printf("Network ordered port: %#x\n", net_port);

    unsigned long host_addr = 0x12345678;
    unsigned long net_addr = htonl(host_addr);
    printf("Host ordered address: %#lx\n", host_addr);
    printf("Network ordered address: %#lx\n", net_addr);

    WSACleanup();

    return 0;
}

// gcc 03.endian_conv_win.c -o 03.endian_conv_win -lws2_32 && 03.endian_conv_win
