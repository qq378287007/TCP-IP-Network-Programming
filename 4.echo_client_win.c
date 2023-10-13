#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define IP "127.0.0.1"
#define PORT 9999
#define BUF_SIZE 1024

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
		ErrorHanding("socket() error");

	int szAddr = sizeof(SOCKADDR_IN);

	SOCKADDR_IN servAddr;
	memset(&servAddr, 0, szAddr);
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(IP);
	servAddr.sin_port = htons(PORT);

	if (connect(hSock, (SOCKADDR *)&servAddr, szAddr) == SOCKET_ERROR)
		ErrorHanding("connect() error");
	else
		puts("Connected.......");

	while (1)
	{

		fputs("Input message(Q to quit): ", stdout);
		char message[BUF_SIZE] = {0};
		fgets(message, BUF_SIZE, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		int strLen = send(hSock, message, strlen(message), 0);

		int recvLen = 0;
		while (recvLen < strLen)
		{
			int recvCnt = recv(hSock, &message[recvLen], BUF_SIZE - 1 - recvLen, 0);
			if (recvCnt == SOCKET_ERROR)
				ErrorHanding("recv() error");
			recvLen += recvCnt;
		}
		message[strLen] = 0;
		printf("Message from server: %s", message);
	}

	closesocket(hSock);

	WSACleanup();

	return 0;
}