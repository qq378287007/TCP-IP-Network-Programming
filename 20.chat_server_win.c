#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define PORT 9999
#define BUF_SIZE 100
#define MAX_CLNT 256

HANDLE mutx;

int clnt_cnt = 0;
SOCKET clnt_socks[MAX_CLNT];

void ErrorHanding(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void send_msg(SOCKET clnt_sock, char *msg, int len)
{
	WaitForSingleObject(mutx, INFINITE);
	for (int i = 0; i < clnt_cnt; i++)
		if (clnt_socks[i] != clnt_sock)
			send(clnt_socks[i], msg, len, 0);
	ReleaseMutex(mutx);
}

unsigned WINAPI handle_clnt(void *arg)
{
	SOCKET clnt_sock = *((SOCKET *)arg);

	char msg[BUF_SIZE];
	int str_len;
	while ((str_len = recv(clnt_sock, msg, sizeof(msg), 0)) != 0)
		send_msg(clnt_sock, msg, str_len);
	closesocket(clnt_sock);

	WaitForSingleObject(mutx, INFINITE);
	for (int i = 0; i < clnt_cnt; i++)
		if (clnt_sock == clnt_socks[i])
		{
			for (; i < clnt_cnt - 1; i++)
				clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	clnt_cnt--;
	ReleaseMutex(mutx);

	return 0;
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHanding("WSAStartup() error!");

	mutx = CreateMutex(NULL, FALSE, NULL);

	SOCKET serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == INVALID_SOCKET)
		ErrorHanding("socket() error");

	int opt = 1;
	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
		ErrorHanding("setsockopt() error");

	int addr_size = sizeof(SOCKADDR_IN);

	SOCKADDR_IN serv_addr;
	memset(&serv_addr, 0, addr_size);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if (bind(serv_sock, (SOCKADDR *)&serv_addr, addr_size) == SOCKET_ERROR)
		ErrorHanding("bind() error");

	if (listen(serv_sock, 5) == SOCKET_ERROR)
		ErrorHanding("listen() error");

	while (1)
	{
		SOCKADDR_IN clnt_addr;
		SOCKET clnt_sock = accept(serv_sock, (SOCKADDR *)&clnt_addr, &addr_size);

		WaitForSingleObject(mutx, INFINITE);
		clnt_socks[clnt_cnt++] = clnt_sock;
		ReleaseMutex(mutx);

		HANDLE t_id = (HANDLE)_beginthreadex(NULL, 0, handle_clnt, (void *)&clnt_sock, 0, NULL);

		printf("Connected client IP: %s:%d\n", inet_ntoa(clnt_addr.sin_addr), clnt_addr.sin_port);
	}

	closesocket(serv_sock);

	CloseHandle(mutx);

	WSACleanup();

	return 0;
}