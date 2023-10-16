#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 9999

void error_handling(char *message)
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

	char message[30];
	int str_len = read(sock, message, sizeof(message) - 1);
	if (str_len == -1)
		error_handling("read() error!");
	message[str_len] = '\0';

	printf("Message from server : %s\n", message);

	close(sock);

	return 0;
}
