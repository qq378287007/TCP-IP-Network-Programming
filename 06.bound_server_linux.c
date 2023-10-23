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
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, addr_size);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&serv_addr, addr_size) == -1)
        error_handling("bind() error");

    for (int i = 0; i < 3; i++)
    {
        sleep(5);

        char message[BUF_SIZE] = {0};
        struct sockaddr_in clnt_addr;
        int str_len = recvfrom(sock, message, BUF_SIZE - 1, 0, (struct sockaddr *)&clnt_addr, &addr_size);
        message[str_len] = 0;
        printf("Message %d: %s\n", i + 1, message);
    }

    close(sock);

    return 0;
}

// gcc 06.bound_server_linux.c -o 06.bound_server_linux && ./06.bound_server_linux
