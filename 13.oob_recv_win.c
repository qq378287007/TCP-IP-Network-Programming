#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 9999
#define BUF_SIZE 30

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    SOCKET listen_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET)
        ErrorHanding("socket() error");

    int opt = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
        ErrorHanding("setsockopt() error");

    int addr_size = sizeof(SOCKADDR_IN);

    SOCKADDR_IN listen_addr;
    memset(&listen_addr, 0, addr_size);
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr.sin_port = htons(PORT);

    if (bind(listen_sock, (struct sockaddr *)&listen_addr, addr_size) == SOCKET_ERROR)
        ErrorHanding("bind() error");

    if (listen(listen_sock, 5) == SOCKET_ERROR)
        ErrorHanding("listen() error");

    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(listen_sock, &reads);

    fd_set excepts;
    FD_ZERO(&excepts);
    FD_SET(listen_sock, &excepts);

    while (1)
    {
        fd_set reads_copy = reads;
        fd_set excepts_copy = excepts;
        struct timeval timeout = {55, 5000};
        int result = select(0, &reads_copy, 0, &excepts_copy, &timeout);
        if (result == SOCKET_ERROR)
            break;
        else if (result == 0)
            continue;

        for (int i = 0; i < reads.fd_count; i++)
        {
            if (FD_ISSET(reads.fd_array[i], &excepts_copy))
            {
                if (reads.fd_array[i] == listen_sock)
                {
                    ErrorHanding("listen_sock error");
                }
                else
                {
                    char buf[BUF_SIZE];
                    // MSG_OOB，一次只发送一个字符
                    // MSG_OOB，一次可以读取多个字符
                    int str_len = recv(reads.fd_array[i], buf, BUF_SIZE - 1, MSG_OOB);
                    buf[str_len] = 0;
                    printf("Urgent message: %s\n", buf);
                }
            }
            
            if (FD_ISSET(reads.fd_array[i], &reads_copy))
            {
                if (reads.fd_array[i] == listen_sock)
                {
                    SOCKADDR_IN connect_addr;
                    SOCKET connect_sock = accept(listen_sock, (SOCKADDR *)&connect_addr, &addr_size);
                    FD_SET(connect_sock, &reads);
                    FD_SET(connect_sock, &excepts);
                    printf("connected client: %d\n", connect_sock);
                }
                else
                {
                    char buf[BUF_SIZE];
                    int str_len = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
                    if (str_len == 0)
                    {
                        FD_CLR(reads.fd_array[i], &reads);
                        FD_CLR(reads.fd_array[i], &excepts);
                        closesocket(reads.fd_array[i]);
                        printf("closed client: %d\n", reads.fd_array[i]);
                    }
                    else
                    {
                        buf[str_len] = 0;
                        puts(buf);
                    }
                }
            }
        }
    }

    closesocket(listen_sock);

    WSACleanup();

    return 0;
}
