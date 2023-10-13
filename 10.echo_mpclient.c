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

void read_routine(int sock)
{
    int str_len;
    char buf[BUF_SIZE];
    while ((str_len = read(sock, buf, BUF_SIZE - 1)) > 0)
    {
        buf[str_len] = 0;
        printf("Message from server: %s\n", buf);
    }
}

void write_routine(int sock)
{
    while (1)
    {
        char buf[BUF_SIZE] = {0};
        fgets(buf, BUF_SIZE - 1, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
        {
            shutdown(sock, SHUT_WR);
            return;
        }
        if (write(sock, buf, strlen(buf)) <= 0)
            break;
    }
}

int main(int argc, char *argv[])
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in addr;
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&addr, addr_size) == -1)
        error_handling("connect() error");

    pid_t pid = fork();
    if (pid == 0)
        write_routine(sock);
    else
        read_routine(sock);

    close(sock);

    return 0;
}