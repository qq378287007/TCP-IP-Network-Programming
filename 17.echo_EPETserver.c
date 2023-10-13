#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 9999
#define BUF_SIZE 4
#define EPOLL_SIZE 50

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

//非阻塞
void setnonblockingmode(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

int main(int argc, char *argv[])
{
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

    struct epoll_event *ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    int epfd = epoll_create(EPOLL_SIZE);

    setnonblockingmode(serv_sock);
    struct epoll_event event;
    event.events = EPOLLIN; //读取事件
    event.data.fd = serv_sock;

    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while (1)
    {
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1)
        {
            puts("epoll_wait() error!");
            break;
        }

        puts("return epoll_wait");

        for (int i = 0; i < event_cnt; i++)
        {
            if (ep_events[i].data.fd == serv_sock)
            {
                struct sockaddr_in clnt_addr;
                int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
                setnonblockingmode(clnt_sock);
                event.events = EPOLLIN | EPOLLET; //读取事件且边缘触发
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d\n", clnt_sock);
            }
            else
            {
                while (1)
                {

                    char buf[BUF_SIZE];
                    int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                    if (str_len == 0)
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                        close(ep_events[i].data.fd);
                        printf("closed client: %d\n", ep_events[i].data.fd);
                        break;
                    }
                    else if (str_len < 0)
                    {
                        if (errno == EAGAIN)
                            break;
                    }
                    else
                    {
                        write(ep_events[i].data.fd, buf, str_len);
                    }
                }
            }
        }
    }

    close(serv_sock);

    close(epfd);
    free(ep_events);

    return 0;
}