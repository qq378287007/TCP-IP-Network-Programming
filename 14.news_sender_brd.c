#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "255.255.255.255"
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
    int send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (send_sock == -1)
        error_handling("socket() error");

    int opt = 1;
    if (setsockopt(send_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error");

    int so_brd = 1;
    if (setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void *)&so_brd, sizeof(so_brd)) == -1)
        error_handling("setsockopt() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in brd_addr;
    memset(&brd_addr, 0, addr_size);
    brd_addr.sin_family = AF_INET;
    brd_addr.sin_addr.s_addr = inet_addr(IP); // Broad IP
    brd_addr.sin_port = htons(PORT);          // Broad Port

    FILE *fp;
    if ((fp = fopen("news_sender_brd.c", "r")) == NULL)
        error_handling("fopen() error");
    while (!feof(fp))
    {
        char buf[BUF_SIZE] = {0};
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&brd_addr, addr_size);
        usleep(200000);
    }
    fclose(fp);

    close(send_sock);

    return 0;
}