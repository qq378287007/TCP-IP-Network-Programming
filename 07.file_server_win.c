#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

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

	SOCKET serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
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

	SOCKADDR_IN clnt_addr;
	SOCKET clnt_sock = accept(serv_sock, (SOCKADDR *)&clnt_addr, &addr_size);
	if (clnt_sock == INVALID_SOCKET)
		ErrorHanding("accept() error");

	FILE *fp = fopen("07.file_server_win.c", "rb");
	int read_cnt;
	char buf[BUF_SIZE];
	while (read_cnt = fread((void *)buf, 1, BUF_SIZE, fp))
		send(clnt_sock, buf, read_cnt, 0);
	fclose(fp);

	//关闭输出流，不再向客户端发送数据
	shutdown(clnt_sock, SD_SEND);

	//recv(clnt_sock, buf, BUF_SIZE, 0);
	while (read_cnt = recv(clnt_sock, buf, BUF_SIZE - 1, 0))
	{
		buf[read_cnt] = 0;
		printf("Message from client: %s\n", buf);
	}

	// close(clnt_sock);
	shutdown(clnt_sock, SD_RECEIVE);

	closesocket(serv_sock);

	WSACleanup();

	return 0;
}

// gcc 07.file_server_win.c -o 07.file_server_win -lws2_32 && 07.file_server_win
