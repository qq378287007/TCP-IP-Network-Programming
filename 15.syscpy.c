#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 3

int main(int argc, char *argv[])
{
    int fd1 = open("syscpy.c", O_RDONLY);
    int fd2 = open("syscpy.c.bak", O_WRONLY | O_CREAT | O_TRUNC);

    char buf[BUF_SIZE];
    int len;
    while ((len = read(fd1, buf, sizeof(buf))) > 0)
        write(fd2, buf, len);

    close(fd2);
    close(fd1);

    return 0;
}