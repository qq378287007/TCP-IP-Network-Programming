#include <stdio.h>

#define BUF_SIZE 3

int main(int argc, char *argv[])
{
    FILE *fp1 = fopen("15.stdcpy.c", "r");
    FILE *fp2 = fopen("15.stdcpy.c.bak", "w");

    char buf[BUF_SIZE];
    while ((fgets(buf, sizeof(buf), fp1)) != NULL)
        fputs(buf, fp2);

    fclose(fp2);
    fclose(fp1);
    return 0;
}

// gcc 15.stdcpy.c -o 15.stdcpy && ./15.stdcpy
