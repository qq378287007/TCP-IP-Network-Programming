#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 3

int main(int argc, char *argv[])
{
    int fd1 = open("15.syscpy.c", O_RDONLY);
    int fd2 = open("15.syscpy.c.bak", O_WRONLY | O_CREAT | O_TRUNC);

    char buf[BUF_SIZE];
    int len;
    while ((len = read(fd1, buf, sizeof(buf))) > 0)
        write(fd2, buf, len);

    close(fd2);
    close(fd1);

    return 0;
}

// gcc 15.syscpy.c -o 15.syscpy && ./15.syscpy