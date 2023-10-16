#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9999

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
        error_handling("socket() error!");

    const int opt = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error!");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, addr_size);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, addr_size) == -1)
        error_handling("bind() error!");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error!");

    struct sockaddr_in clnt_addr;
    int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
    if (clnt_sock == -1)
        error_handling("accept() error!");

    char message[] = "Hello World!";
    write(clnt_sock, message, sizeof(message) - 1);
    close(clnt_sock);

    close(serv_sock);

    return 0;
}
