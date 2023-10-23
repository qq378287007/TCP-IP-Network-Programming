#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
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

    struct sockaddr_in addr;
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    // 向UDP套接字注册目标IP和端口信息
    connect(sock, (struct sockaddr *)&addr, addr_size);

    while (1)
    {
        fputs("Input message(Q to quit): ", stdout);
        char message[BUF_SIZE] = {0};
        fgets(message, BUF_SIZE - 1, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        // sendto(sock, message, strlen(message), 0, (struct sockaddr*)&addr, addr_size);
        write(sock, message, strlen(message));

        //int str_len = recvfrom(sock, message, BUF_SIZE-1, 0, (struct sockaddr*)&addr, &addr_size);
        int str_len = read(sock, message, sizeof(message) - 1);
        message[str_len] = 0;
        printf("Message from server : %s\n", message);
    }

    close(sock);

    return 0;
}

// gcc 06.uecho_con_client_linux.c -o 06.uecho_con_client_linux && ./06.uecho_con_client_linux
