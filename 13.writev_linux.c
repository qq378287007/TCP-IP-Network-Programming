#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char *argv[])
{
    struct iovec vec[2];

    char buf1[] = "ABCDEFG";
    vec[0].iov_base = buf1;
    vec[0].iov_len = 3;

    char buf2[] = "1234567";
    vec[1].iov_base = buf2;
    vec[1].iov_len = 4;

    int str_len = writev(1, vec, 2);
    puts("");
    printf("Write bytes: %d\n", str_len);
    
    return 0;
}

// gcc 13.writev_linux.c -o 13.writev_linux && ./13.writev_linux
