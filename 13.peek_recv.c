#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9999
#define BUF_SIZE 30

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (acpt_sock == -1)
        error_handling("socket() error");

    int opt = 1;
    if (setsockopt(acpt_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in acpt_adr;
    memset(&acpt_adr, 0, addr_size);
    acpt_adr.sin_family = AF_INET;
    acpt_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    acpt_adr.sin_port = htons(PORT);

    if (bind(acpt_sock, (struct sockaddr *)&acpt_adr, addr_size) == -1)
        error_handling("bind() error");

    if (listen(acpt_sock, 5) == -1)
        error_handling("listen() error");

    struct sockaddr_in recv_adr;
    int connect_sock = accept(acpt_sock, (struct sockaddr *)&recv_adr, &addr_size);

    int str_len;
    char buf[BUF_SIZE];

    while (1)
    {
        str_len = recv(connect_sock, buf, sizeof(buf) - 1, MSG_PEEK | MSG_DONTWAIT);
        if (str_len > 0)
            break;
    }
    buf[str_len] = 0;
    printf("Buffering %d bytes: %s\n", str_len, buf);

    str_len = recv(connect_sock, buf, sizeof(buf) - 1, 0);
    buf[str_len] = 0;
    printf("Read again: %s\n", buf);

    close(connect_sock);

    close(acpt_sock);

    return 0;
}