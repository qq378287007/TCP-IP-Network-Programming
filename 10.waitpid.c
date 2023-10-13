#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid = fork();
    if (pid == 0)
    {
        sleep(5);
        return 24;
    }
    else
    {
        printf("Child PID: %d\n", pid);

        int status;
        while (!waitpid(-1, &status, WNOHANG))
        {
            sleep(3);
            puts("sleep 3sec.");
        }

        if (WIFEXITED(status))
            printf("Child send %d\n", WEXITSTATUS(status));
        return 0;
    }
}