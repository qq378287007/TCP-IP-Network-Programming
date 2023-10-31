#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int cfd1 = dup(1);
    int cfd2 = dup2(1, 7);

    printf("fd1=%d, fd2=%d\n", cfd1, cfd2);

    char str1[] = "Hi~ \n";
    write(cfd1, str1, sizeof(str1));

    char str2[] = "nice day~ \n";
    write(cfd2, str2, sizeof(str2));

    close(cfd1);
    close(cfd2);

    write(1, str1, sizeof(str1));
    close(1);

    write(1, str2, sizeof(str2)); // 所有输出文件描述符已关闭，客户端不再输出

    return 0;
}

// gcc 16.udp.c -o 16.udp && ./16.udp
