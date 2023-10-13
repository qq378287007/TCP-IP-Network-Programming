#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
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
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in addr;
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    char message1[] = "Hi!";
    sendto(sock, message1, strlen(message1), 0, (struct sockaddr *)&addr, addr_size);

    char message2[] = "I'm another UDP host!";
    sendto(sock, message2, strlen(message2), 0, (struct sockaddr *)&addr, addr_size);

    char message3[] = "Nice to meet you!";
    sendto(sock, message3, strlen(message3), 0, (struct sockaddr *)&addr, addr_size);

    close(sock);
    
    return 0;
}