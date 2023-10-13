#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status))
        printf("Removed proc id: %d\n", id);
    printf("Child send %d\n", WEXITSTATUS(status));
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = read_childproc;
    sigaction(SIGCHLD, &act, 0);

    pid_t pid = fork();
    if (pid == 0)
    {
        puts("Hi! I'm child process");
        sleep(2);
        return 12;
    }
    else
    {
        printf("Child PID: %d\n", pid);

        pid = fork();
        if (pid == 0)
        {
            puts("Hi! I'm child process");
            sleep(4);
            exit(24);
        }
        else
        {
            printf("Child PID: %d\n", pid);
            for (int i = 0; i < 5; i++)
            {
                puts("wait...");
                sleep(3);
            }
            return 0;
        }
    }
}