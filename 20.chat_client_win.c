#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <time.h>

#define IP "127.0.0.1"
#define PORT 9999
#define BUF_SIZE 1024
#define NAME_SIZE 20

char *gen_random_str(char *str, int length)
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < length; i++)
	{
		switch (rand() % 2)
		{
		case 0:
			str[i] = 'A' + rand() % 26;
			break;
		case 1:
			str[i] = 'a' + rand() % 26;
			break;
		}
	}
	str[length] = '\0';
	return str;
}

void ErrorHanding(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

unsigned WINAPI send_msg(void *arg)
{
	char name[NAME_SIZE] = "DEFAULT";
	gen_random_str(name, 7);

	SOCKET sock = *((SOCKET *)arg);

	while (1)
	{
		char msg[BUF_SIZE] = {0};
		fgets(msg, BUF_SIZE - 1, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(sock);
			exit(0);
		}
		char name_msg[NAME_SIZE + BUF_SIZE + 3];
		sprintf(name_msg, "[%s] %s", name, msg);
		send(sock, name_msg, strlen(name_msg), 0);
	}
	return 0;
}

unsigned WINAPI recv_msg(void *arg)
{
	SOCKET sock = *((SOCKET *)arg);
	while (1)
	{
		char name_msg[NAME_SIZE + BUF_SIZE] = {0};
		int str_len = recv(sock, name_msg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (str_len == -1)
			return -1;
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHanding("WSAStartup() error!");

	SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		ErrorHanding("socket() error");

	int addr_size = sizeof(SOCKADDR_IN);

	SOCKADDR_IN addr;
	memset(&addr, 0, addr_size);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);

	if (connect(sock, (SOCKADDR *)&addr, addr_size) == SOCKET_ERROR)
		ErrorHanding("connect() error");

	HANDLE snd_thread = (HANDLE)_beginthreadex(NULL, 0, send_msg, (void *)&sock, 0, NULL);
	HANDLE rcv_thread = (HANDLE)_beginthreadex(NULL, 0, recv_msg, (void *)&sock, 0, NULL);

	WaitForSingleObject(snd_thread, INFINITE);
	WaitForSingleObject(rcv_thread, INFINITE);

	closesocket(sock);

	WSACleanup();

	return 0;
}