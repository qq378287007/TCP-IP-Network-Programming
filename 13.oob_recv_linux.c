#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 9999
#define BUF_SIZE 30

int recv_sock;

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void urg_handler(int signo)
{
    char buf[BUF_SIZE];
    //int str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_OOB); //一次urg_handler调用只能读取1个字节
    int str_len = recv(recv_sock, buf, 1, MSG_OOB); 
    buf[str_len] = 0;
    printf("Urgent message: %s\n", buf);
}

int main(int argc, char *argv[])
{
    int acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (acpt_sock == -1)
        error_handling("socket() error");

	int opt = 1;
	if (setsockopt(acpt_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
		error_handling("setsockopt() error");

    socklen_t addr_size = sizeof(struct sockaddr_in);

    struct sockaddr_in recv_adr;
    memset(&recv_adr, 0, addr_size);
    recv_adr.sin_family = AF_INET;
    recv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_adr.sin_port = htons(PORT);

    if (bind(acpt_sock, (struct sockaddr *)&recv_adr, addr_size) == -1)
        error_handling("bind() error");

    if (listen(acpt_sock, 5) == -1)
        error_handling("listen() error");

    struct sockaddr_in serv_adr;
    recv_sock = accept(acpt_sock, (struct sockaddr *)&serv_adr, &addr_size);

    // fcntl函数控制文件描述符
    //指定recv_sock套接字的拥有者（F_SETOWN）为getpid()表示的进程
    //套接字的拥有者实际为操作系统
    //这里指负责套接字所有事务的主体
    //处理SIGURG信号的主体为当前进程，并非所有进程都会处理SIGURG信号
    //文件描述符recv_sock指向的套接字引发的SIGURG信号处理进程变为将getpid函数返回值用作ID的进程
    fcntl(recv_sock, F_SETOWN, getpid());

    struct sigaction act;
    act.sa_handler = urg_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    int state = sigaction(SIGURG, &act, 0); //收到MSG_OOB紧急信息将产生SIGURG信号

    char buf[BUF_SIZE];
    int str_len;
    while ((str_len = recv(recv_sock, buf, sizeof(buf) - 1, 0)) != 0)
    {
        if (str_len == -1)
            continue;
        buf[str_len] = 0;
        puts(buf);
    }

    close(recv_sock);

    close(acpt_sock);

    return 0;
}

// gcc 13.oob_recv_linux.c -o 13.oob_recv_linux && ./13.oob_recv_linux
