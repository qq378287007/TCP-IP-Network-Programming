#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
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

	while (1)
	{
		fputs("Input message(Q to quit): ", stdout);
		char message[BUF_SIZE] = {0};
		fgets(message, BUF_SIZE - 1, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		int str_len = write(sock, message, strlen(message));
		int recv_len = 0;
		while (recv_len < str_len)
		{
			int recv_cnt = read(sock, &message[recv_len], BUF_SIZE - 1 - recv_len);
			if (recv_cnt == -1)
				error_handling("read() error!");
			recv_len += recv_cnt;
		}
		message[recv_len] = 0;
		printf("Message from server: %s\n", message);
	}

	close(sock);

	return 0;
}

// gcc 04.echo_client_linux.c -o 04.echo_client_linux && ./04.echo_client_linux
