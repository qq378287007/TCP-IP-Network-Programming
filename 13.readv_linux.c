#include <stdio.h>
#include <sys/uio.h>

#define BUF_SIZE 100

int main(int argc, char *argv[])
{
    struct iovec vec[2];

    char buf1[BUF_SIZE] = {0};
    vec[0].iov_base = buf1;
    vec[0].iov_len = 5;

    char buf2[BUF_SIZE] = {0};
    vec[1].iov_base = buf2;
    vec[1].iov_len = BUF_SIZE;

    int str_len = readv(0, vec, 2);
    printf("Read bytes: %d\n", str_len);
    printf("First message: %s\n", buf1);
    printf("Second message: %s\n", buf2);

    return 0;
}

// gcc 13.readv_linux.c -o 13.readv_linux && ./13.readv_linux
