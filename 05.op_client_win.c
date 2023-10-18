#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define IP "127.0.0.1"
#define PORT 9999

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

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

	SOCKET hSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hSock == INVALID_SOCKET)
		ErrorHanding("socket() error!");

	int szAddr = sizeof(SOCKADDR_IN);

	SOCKADDR_IN servAddr;
	memset(&servAddr, 0, szAddr);
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(IP);
	servAddr.sin_port = htons(PORT);

	if (connect(hSock, (SOCKADDR *)&servAddr, szAddr) == SOCKET_ERROR)
		ErrorHanding("connect() error!");
	else
		puts("Connected.......");

	int opnd_cnt;
	fputs("Operand count: ", stdout);
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

	send(hSock, opmsg, 1 + OPSZ * opnd_cnt + 1, 0);
	int result;
	recv(hSock, (char *)&result, RLT_SIZE, 0);
	printf("Operation result: %d\n", result);

	closesocket(hSock);

	WSACleanup();

	return 0;
}

// gcc 05.op_client_win.c -o 05.op_client_win -lws2_32 && 05.op_client_win
