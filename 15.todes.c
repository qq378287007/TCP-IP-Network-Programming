#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd = open("data.dat", O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == -1)
    {
        fputs("file open error", stdout);
        return -1;
    }

    printf("first file descriptor: %d\n", fd);
    write(fd, "tcp/ip\n", strlen("tcp/ip\n"));
    // close(fd);

    FILE *fp = fdopen(fd, "w");
    fputs("Network C Programming\n", fp);
    printf("second file descriptor: %d\n", fileno(fp));
    fclose(fp);

    return 0;
}

// gcc 15.todes.c -o 15.todes && ./15.todes