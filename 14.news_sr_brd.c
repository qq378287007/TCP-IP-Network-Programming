#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define IP "255.255.255.255"
#define PORT 9999
#define TTL 64
#define BUF_SIZE 30

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *send_sock(void *arg)
{
    int sock = *(int *)arg;

    int so_brd = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&so_brd, sizeof(so_brd)) == -1)
        error_handling("setsockopt() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in brd_addr;
    memset(&brd_addr, 0, addr_size);
    brd_addr.sin_family = AF_INET;
    brd_addr.sin_addr.s_addr = inet_addr(IP); // Broad IP
    brd_addr.sin_port = htons(PORT);          // Broad Port

    sleep(1);

    FILE *fp;
    if ((fp = fopen("news_sr_brd.c", "r")) == NULL)
        error_handling("fopen() error");
    while (!feof(fp))
    {
        char buf[BUF_SIZE] = {0};
        fgets(buf, BUF_SIZE, fp);
        sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&brd_addr, addr_size);
        usleep(200000);
    }
    fclose(fp);

    close(sock);

    return NULL;
}

void *recv_sock(void *arg)
{
    int sock = *(int *)arg;
    
    socklen_t addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in recv_addr;
    memset(&recv_addr, 0, addr_size);
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(PORT);
    if (bind(sock, (struct sockaddr *)&recv_addr, addr_size) == -1)
        error_handling("bind() error");

    while (1)
    {
        char buf[BUF_SIZE] = {0};
        int str_len = recvfrom(sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len <= 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }

    close(sock);

    return (void *)0;
}

int main(int argc, char *argv[])
{
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error");

    pthread_t t1;
    pthread_create(&t1, NULL, send_sock, (void *)&sock);
    pthread_t t2;
    pthread_create(&t2, NULL, recv_sock, (void *)&sock);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}