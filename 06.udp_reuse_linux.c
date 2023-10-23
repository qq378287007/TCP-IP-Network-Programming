#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <pthread.h>

#define IP "127.0.0.1"
#define PORT 9999
#define BUF_SIZE 30

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *read_udp_message(void *arg)
{
    int sock = *(int *)arg;
    char message[BUF_SIZE];
    while (1)
    {
        int str_len = read(sock, message, sizeof(message) - 1);
        message[str_len] = 0;
        printf("Message from server : %s\n", message);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in addr;
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&addr, addr_size) == -1)
        error_handling("bind() error");

    connect(sock, (struct sockaddr *)&addr, addr_size);

    pthread_t tidp;
    int ret = pthread_create(&tidp, NULL, read_udp_message, &sock);
    while (1)
    {
        char message[BUF_SIZE] = "message";
        write(sock, message, strlen(message));
        sleep(1);
    }
    pthread_join(tidp, NULL);

    close(sock);

    return 0;
}

// gcc 06.udp_reuse_linux.c -o 06.udp_reuse_linux && ./06.udp_reuse_linux
