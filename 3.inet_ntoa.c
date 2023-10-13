#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in addr1;
    addr1.sin_addr.s_addr = htonl(0x01020304);

    struct sockaddr_in addr2;
    addr2.sin_addr.s_addr = htonl(0x01010101);

    char *str_ptr = inet_ntoa(addr1.sin_addr);
    char str_arr[20];
    strcpy(str_arr, str_ptr);
    printf("Dotted-Decimal notation1: %s\n", str_ptr);

    inet_ntoa(addr2.sin_addr);
    printf("Dotted-Decimal notation2: %s\n", str_ptr);
    printf("Dotted-Decimal notation3: %s\n", str_arr);

    return 0;
}