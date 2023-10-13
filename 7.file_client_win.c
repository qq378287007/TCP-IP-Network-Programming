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

	SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		ErrorHanding("socket() error");

	int addr_size = sizeof(SOCKADDR_IN);

	SOCKADDR_IN addr;
	memset(&addr, 0, addr_size);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);

	if (connect(sock, (SOCKADDR *)&addr, addr_size) == SOCKET_ERROR)
		ErrorHanding("connect() error");

	FILE *fp = fopen("receive.dat", "wb");
	char buf[BUF_SIZE];
	int read_cnt;
	while ((read_cnt = recv(sock, buf, BUF_SIZE, 0)) != 0)
		fwrite((void *)buf, 1, read_cnt, fp);
	fclose(fp);

	puts("Received file data");

	send(sock, "Thank you", 10, 0);

	closesocket(sock);

	WSACleanup();

	return 0;
}