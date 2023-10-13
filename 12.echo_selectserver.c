#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define PORT 9999
#define BUF_SIZE 100

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    int opt = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, addr_size);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, addr_size) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    int fd_max = serv_sock;

    while (1)
    {
        fd_set cpy_reads = reads;
        struct timeval timeout = {5, 5000};
        //调用select后，timeout的tv_sec和tv_usec将被替换为超时前剩余时间

        int fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
        if (fd_num == -1)
            break;
        else if (fd_num == 0)
            continue;
        for (int i = 0; i <= fd_max; i++)
        {
            if (FD_ISSET(i, &cpy_reads))
            {
                if (i == serv_sock)
                {
                    struct sockaddr_in clnt_addr;
                    int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
                    FD_SET(clnt_sock, &reads);
                    if (fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("connected client: %d\n", clnt_sock);
                }
                else
                {
                    char buf[BUF_SIZE];
                    int str_len = read(i, buf, BUF_SIZE);
                    if (str_len == 0)
                    {
                        close(i);
                        FD_CLR(i, &reads);
                        printf("closed client: %d\n", i);
                    }
                    else
                    {
                        write(i, buf, str_len);
                    }
                }
            }
        }
    }

    close(serv_sock);

    return 0;
}