#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define URL "www.baidu.com"

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    struct hostent *host = gethostbyname(URL);
    if (!host)
        error_handling("gethostbyname... error");

    printf("Official name: %s\n", host->h_name);

    for (int i = 0; host->h_aliases[i]; i++)
        printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);

    printf("Address type: %s\n",
           host->h_addrtype == AF_INET ? "AF_INET" : "AF_INET6");

    for (int i = 0; host->h_addr_list[i]; i++)
        printf("IP addr %d: %s\n", i + 1,
               inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));

    return 0;
}