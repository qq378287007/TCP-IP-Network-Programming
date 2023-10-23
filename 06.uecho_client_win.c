#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define IP "127.0.0.1"
#define PORT 9999
#define BUF_SIZE 30

void ErrorHanding(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHanding("WSAStartup() error!");

	SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		ErrorHanding("socket() error");

	int addr_size = sizeof(SOCKADDR_IN);

	SOCKADDR_IN addr;
	memset(&addr, 0, addr_size);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);

	// 向UDP套接字注册目标IP和端口信息
	connect(sock, (SOCKADDR *)&addr, addr_size);

	while (1)
	{
		fputs("Input message(Q to quit): ", stdout);
		char message[BUF_SIZE] = {0};
		fgets(message, BUF_SIZE - 1, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		send(sock, message, strlen(message), 0);

		int str_len = recv(sock, message, sizeof(message) - 1, 0);
		message[str_len] = 0;
		printf("Message from server : %s\n", message);
	}

	closesocket(sock);

	WSACleanup();

	return 0;
}

// gcc 06.uecho_client_win.c -o 06.uecho_client_win -lws2_32 && 06.uecho_client_win
