#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 9999
#define BUF_SIZE 100
#define MAX_CLNT 256

pthread_mutex_t mutx = PTHREAD_MUTEX_INITIALIZER;

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];

void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void send_msg(int clnt_sock, char *msg, int len)
{
	pthread_mutex_lock(&mutx);
	for (int i = 0; i < clnt_cnt; i++)
		if (clnt_socks[i] != clnt_sock)
			write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}

void *handle_clnt(void *arg)
{
	int clnt_sock = *(int *)arg;
	char msg[BUF_SIZE];
	int str_len;
	while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
		send_msg(clnt_sock, msg, str_len);
	close(clnt_sock);

	pthread_mutex_lock(&mutx);
	for (int i = 0; i < clnt_cnt; i++)
		if (clnt_sock == clnt_socks[i])
		{
			for (; i < clnt_cnt - 1; i++)
				clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_mutex_init(&mutx, NULL);

	int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		error_handling("socket() error");
	int opt = 1;
	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
		error_handling("setsockopt() error");

	socklen_t addr_size = sizeof(struct sockaddr_in);

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, addr_size);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, addr_size) == -1)
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	while (1)
	{
		struct sockaddr_in clnt_addr;
		int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_t t_id;
		pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
		pthread_detach(t_id);

		printf("Connected client IP: %s:%d\n", inet_ntoa(clnt_addr.sin_addr), clnt_addr.sin_port);
	}

	close(serv_sock);

	pthread_mutex_destroy(&mutx);

	return 0;
}