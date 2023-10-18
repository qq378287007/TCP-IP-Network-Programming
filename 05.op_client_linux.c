#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 9999
#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error!");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in addr;
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&addr, addr_size) == -1)
        error_handling("connect() error!");
    else
        puts("Connected......");

    fputs("Operand count: ", stdout);
    int opnd_cnt;
    scanf("%d", &opnd_cnt);
    char opmsg[BUF_SIZE];
    opmsg[0] = (char)opnd_cnt;

    for (int i = 0; i < opnd_cnt; i++)
    {
        printf("Operand %d: ", i + 1);
        scanf("%d", (int *)&opmsg[1 + OPSZ * i]);
    }

    fgetc(stdin); //'\n'

    fputs("Operator: ", stdout);
    scanf("%c", &opmsg[1 + OPSZ * opnd_cnt]);

    write(sock, opmsg, 1 + OPSZ * opnd_cnt + 1);
    int result;
    read(sock, &result, RLT_SIZE);
    printf("Operation result: %d\n", result);

    close(sock);

    return 0;
}

// gcc 05.op_client_linux.c -o 05.op_client_linux && ./05.op_client_linux