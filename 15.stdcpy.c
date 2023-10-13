#include <stdio.h>

#define BUF_SIZE 3

int main(int argc, char *argv[])
{
    FILE *fp1 = fopen("stdcpy.c", "r");
    FILE *fp2 = fopen("stdcpy.c.bak", "w");

    char buf[BUF_SIZE];
    while ((fgets(buf, sizeof(buf), fp1)) != NULL)
        fputs(buf, fp2);

    fclose(fp2);
    fclose(fp1);
    return 0;
}