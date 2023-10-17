#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    char *addr1 = "1.2.3.4";
    unsigned long conv_addr = inet_addr(addr1);
    if (conv_addr == INADDR_NONE)
        printf("Error occured!\n");
    else
        printf("Network ordered integer addr: %#lx\n", conv_addr);

    char *addr2 = "1.2.3.256";
    conv_addr = inet_addr(addr2);
    if (conv_addr == INADDR_NONE)
        printf("Error occured!\n");
    else
        printf("Network ordered integer addr: %#lx\n", conv_addr);

    return 0;
}

// gcc 03.inet_addr_linux.c -o 03.inet_addr_linux && ./03.inet_addr_linux
