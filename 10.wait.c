#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid = fork();
    if (pid == 0)
    {
        sleep(3);
        return 3;
    }
    else
    {
        printf("Child PID: %d\n", pid);

        pid = fork();
        if (pid == 0)
        {
            sleep(2);
            exit(7);
        }
        else
        {
            printf("Child PID: %d\n", pid);

            // sleep(10);
            int status;
            wait(&status);
            if (WIFEXITED(status))
                printf("Child send one: %d\n", WEXITSTATUS(status));

            wait(&status);
            if (WIFEXITED(status))
                printf("Child send two: %d\n", WEXITSTATUS(status));

            // sleep(10);
            return 0;
        }
    }
}

// gcc 10.wait.c -o 10.wait && ./10.wait
