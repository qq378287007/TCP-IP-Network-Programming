#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9999

#define BUF_SIZE 1024
#define OPSZ 4

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int calculate(int opnum, int opnds[], char op)
{
    int result = opnds[0];
    int i;
    switch (op)
    {
    case '+':
        for (i = 1; i < opnum; i++)
            result += opnds[i];
        break;
    case '-':
        for (i = 1; i < opnum; i++)
            result -= opnds[i];
        break;
    case '*':
        for (i = 1; i < opnum; i++)
            result *= opnds[i];
        break;
    }
    return result;
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

    for (int i = 0; i < 5; i++)
    {
        struct sockaddr_in clnt_addr;
        int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error!");
        else
            printf("Connected client %d\n", i + 1);

        int opnd_cnt = 0;
        //接收一个字节，数值个数
        read(clnt_sock, &opnd_cnt, 1);

        char opinfo[BUF_SIZE];
        //接收opnd_cnt个数值+1个运算符
        int recv_len = 0;
        while (recv_len < OPSZ * opnd_cnt + 1)
        {
            int recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE - 1 - recv_len);
            recv_len += recv_cnt;
        }

        int result = calculate(opnd_cnt, (int *)opinfo, opinfo[recv_len - 1]);
        write(clnt_sock, (char *)&result, sizeof(result));

        close(clnt_sock);
    }

    close(serv_sock);

    return 0;
}

// gcc 05.op_server_linux.c -o 05.op_server_linux && ./05.op_server_linux
