#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    unsigned short host_port = 0x1234;
    unsigned short net_port = htons(host_port);
    printf("Host ordered port: %#x\n", host_port);
    printf("Network ordered port: %#x\n", net_port);

    unsigned long host_addr = 0x12345678;
    unsigned long net_addr = htonl(host_addr);
    printf("Host ordered address: %#lx \n", host_addr);
    printf("Network ordered address: %#lx \n", net_addr);

    return 0;
}

// gcc 03.endian_conv_linux.c -o 03.endian_conv_linux && ./03.endian_conv_linux
