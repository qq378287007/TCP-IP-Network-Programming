#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

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

void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void *send_msg(void *arg)
{
	char name[NAME_SIZE] = "DEFAULT";
	gen_random_str(name, 7);

	int sock = *((int *)arg);
	while (1)
	{
		char msg[BUF_SIZE] = {0};
		fgets(msg, BUF_SIZE - 1, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			close(sock);
			exit(0);
		}
		char name_msg[NAME_SIZE + BUF_SIZE+3] = {0};
		sprintf(name_msg, "[%s] %s", name, msg);
		write(sock, name_msg, strlen(name_msg));
	}
	return NULL;
}

void *recv_msg(void *arg)
{
	int sock = *((int *)arg);
	while (1)
	{
		char name_msg[NAME_SIZE + BUF_SIZE] = {0};
		int str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
		if (str_len == -1)
			return (void *)-1;
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	socklen_t addr_size = sizeof(struct sockaddr_in);

	struct sockaddr_in addr;
	memset(&addr, 0, addr_size);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);

	if (connect(sock, (struct sockaddr *)&addr, addr_size) == -1)
		error_handling("connect() error");

	pthread_t snd_thread;
	pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);

	pthread_t rcv_thread;
	pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);

	void *thread_return;
	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	close(sock);

	return 0;
}