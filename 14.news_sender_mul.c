#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "224.1.1.2"
#define PORT 9999
#define TTL 64
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

    int time_live = TTL;
    if (setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&time_live, sizeof(time_live)) == -1)
        error_handling("setsockopt() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in mul_addr;
    memset(&mul_addr, 0, addr_size);
    mul_addr.sin_family = AF_INET;
    mul_addr.sin_addr.s_addr = inet_addr(IP); // Multicast IP
    mul_addr.sin_port = htons(PORT);          // Multicast Port

    FILE *fp;
    if ((fp = fopen("news_sender_mul.c", "r")) == NULL)
        error_handling("fopen() error");
    while (!feof(fp))
    {
        char buf[BUF_SIZE] = {0};
        fgets(buf, BUF_SIZE - 1, fp);
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&mul_addr, addr_size);
        usleep(200000);
    }
    fclose(fp);

    close(send_sock);

    return 0;
}