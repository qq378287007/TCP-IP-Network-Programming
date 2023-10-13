#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    int udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    printf("SOCK_STREAM: %d\n", SOCK_STREAM);
    printf("SOCK_DGRAM: %d\n", SOCK_DGRAM);

    int sock_type;
    socklen_t optLen = sizeof(sock_type);
    int state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void *)&sock_type, &optLen);
    if (state)
        error_handling("getsockopt() error");
    printf("Socket type one: %d\n", sock_type);

    state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void *)&sock_type, &optLen);
    if (state)
        error_handling("getsockopt() error");
    printf("Socket type two: %d\n", sock_type);

    close(udp_sock);
    close(tcp_sock);

    return 0;
}