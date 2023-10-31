#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_main(void *arg)
{
	int cnt = *((int *)arg);
	for (int i = 0; i < cnt; i++)
	{
		sleep(1);
		puts("runnint thread");
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int thread_param = 5;
	pthread_t t_id;
	if (pthread_create(&t_id, NULL, thread_main, (void *)&thread_param) != 0)
	{
		puts("pthread_create error");
		return -1;
	}

	sleep(10);
	puts("end of main");
	return 0;
};

// gcc 18.thread1.c -o 18.thread1 && ./18.thread1
