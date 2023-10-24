#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char *argv[])
{
	pid_t pid = fork();

	if (pid == 0)
	{
		puts("Hi, I am a child process");
	}
	else
	{
		printf("Child Process ID: %d\n", pid);
		sleep(10); // 父进程暂停10秒，验证运行结束的处于僵尸状态的子进程是否销毁
	}

	if (pid == 0)
		puts("End child process");
	else // 父进程结束，僵尸状态的子进程同时销毁
		puts("End parent process");

	return 0;
}

// gcc 10.zombie.c -o 10.zombie && ./10.zombie
