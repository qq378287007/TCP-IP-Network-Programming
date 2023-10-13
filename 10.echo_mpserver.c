#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 9999
#define BUF_SIZE 30

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childprod(int sig)
{
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d\n", pid);
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = read_childprod;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    int state = sigaction(SIGCHLD, &act, 0);

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

    while (1)
    {
        struct sockaddr_in clnt_addr;
        int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
        if (clnt_sock == -1)
            continue;
        else
            puts("new client connected...");

        pid_t pid = fork();
        if (pid == -1)
        {
            close(clnt_sock);
            continue;
        }
        else if (pid == 0)
        {
            close(serv_sock);

            int str_len;
            char buf[BUF_SIZE];
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
                write(clnt_sock, buf, str_len);

            close(clnt_sock);

            puts("client disconnected...");
            return 0;
        }
        else
        {
            close(clnt_sock);
        }
    }

    close(serv_sock);

    return 0;
}