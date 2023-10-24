#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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
    int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (recv_sock == -1)
        error_handling("socket() error!");

    int opt = 1;
    if (setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error!");

    socklen_t addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in recv_addr;
    memset(&recv_addr, 0, addr_size);
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(PORT);
    if (bind(recv_sock, (struct sockaddr *)&recv_addr, addr_size) == -1)
        error_handling("bind() error!");

    while (1)
    {
        char buf[BUF_SIZE] = {0};
        int str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len <= 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }

    close(recv_sock);

    return 0;
}

// gcc 14.news_receiver_brd_linux.c -o 14.news_receiver_brd_linux && ./14.news_receiver_brd_linux
