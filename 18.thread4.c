#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100

long long num = 0;

void *thread_inc(void *arg)
{
	for (int i = 0; i < 50000000; i++)
		num += 1;
	return NULL;
}

void *thread_des(void *arg)
{
	for (int i = 0; i < 50000000; i++)
		num -= 1;
	return (void *)0;
}

int main(int argc, char *argv[])
{
	printf("size long long: %ld\n", sizeof(long long));
	pthread_t thread_id[NUM_THREAD];
	for (int i = 0; i < NUM_THREAD; i++)
		if (i % 2)
			pthread_create(&thread_id[i], NULL, thread_inc, NULL);
		else
			pthread_create(&thread_id[i], NULL, thread_des, NULL);

	for (int i = 0; i < NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);

	printf("result: %lld\n", num);
	return 0;
};

// gcc 18.thread4.c -D_REENTRANT -o 18.thread4 && ./18.thread4
