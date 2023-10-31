#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd = open("data.dat", O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == -1)
    {
        fputs("file open error", stdout);
        return -1;
    }

    FILE *fp = fdopen(fd, "w");
    fputs("Network C Programming\n", fp);
    fclose(fp);
    // FILE指针关闭文件后，无需通过文件描述符关闭（close(fd);）
    // 文件描述符fd变成无意义的整数

    return 0;
}

// gcc 15.desto.c -o 15.desto && ./15.desto
// cat data.dat