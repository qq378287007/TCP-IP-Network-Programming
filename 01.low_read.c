#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 128

void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char *argv[])
{
	int fd = open("data.txt", O_RDONLY);
	if (fd == -1)
		error_handling("open() error");
	printf("file descriptor: %d\n", fd);

	char buf[BUF_SIZE];
	int strLen = read(fd, buf, sizeof(buf) - 1);
	if (strLen == -1)
		error_handling("read() error");
	buf[strLen] = '\0';
	printf("file data: %s\n", buf);

	close(fd);

	return 0;
}
