#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9999
#define BUF_SIZE 1024

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

    struct sockaddr_in clnt_addr;
    int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
    if (clnt_sock == -1)
        error_handling("accept() error");

    FILE *readfp = fdopen(clnt_sock, "r");
    FILE *writefp = fdopen(dup(clnt_sock), "w");

    fputs("FROM SERVR: Hi~ client?\n", writefp);
    fputs("I love all of the world.\n", writefp);
    fputs("You are awesome!\n", writefp);
    fflush(writefp);
    
	//调用shutdown函数，无论复制出多少文件描述符
	//都进入半关闭状态，同时传递EOF
	shutdown(fileno(writefp), SHUT_WR);
    fclose(writefp);

    char buf[BUF_SIZE] = {0};
    fgets(buf, BUF_SIZE - 1, readfp);
    fputs(buf, stdout);
    fclose(readfp);

    close(serv_sock);

    return 0;
}