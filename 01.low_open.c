#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char *argv[])
{
	int fd = open("data.txt", O_CREAT | O_WRONLY | O_TRUNC);
	if (fd == -1)
		error_handling("open() error!");
	printf("file descriptor: %d\n", fd);

	char buf[] = "Let's go!\n";
	if (write(fd, buf, strlen(buf)) == -1)
		error_handling("write() error");

	close(fd);

	return 0;
}

// gcc 01.low_open.c -o 01.low_open && ./01.low_open && cat data.txt
