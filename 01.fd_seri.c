#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
void ErrorHanding(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
#else
#include <sys/socket.h>
#endif

int main(int argc, char *argv[])
{
	int fd2 = open("test.dat", O_CREAT | O_WRONLY | O_TRUNC);
	printf("file descriptor 2: %d\n", fd2);
	close(fd2);

#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHanding("WSAStartup() error!");

	SOCKET fd1 = socket(PF_INET, SOCK_STREAM, 0);
	if (fd1 == INVALID_SOCKET)
		ErrorHanding("socket() error");
	SOCKET fd3 = socket(PF_INET, SOCK_STREAM, 0);
	if (fd3 == INVALID_SOCKET)
		ErrorHanding("socket() error");

	printf("file descriptor 1: %llu\n", fd1);
	printf("file descriptor 3: %llu\n", fd3);

	closesocket(fd1);
	closesocket(fd3);

	WSACleanup();
#else
	int fd1 = socket(PF_INET, SOCK_STREAM, 0);
	int fd3 = socket(PF_INET, SOCK_STREAM, 0);
	printf("file descriptor 1: %d\n", fd1);
	printf("file descriptor 3: %d\n", fd3);
	close(fd1);
	close(fd3);
#endif

	return 0;
}