#include <stdio.h>
#include <pthread.h>

int sum = 0;

void *thread_summation(void *arg)
{
	int *p = (int *)arg;
	int end = p[1];
	for (int start = p[0]; start <= end; start++)
		sum += start;

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t id_t1;
	int range1[] = {1, 500};
	pthread_create(&id_t1, NULL, thread_summation, (void *)&range1);

	pthread_t id_t2;
	int range2[] = {501, 1000};
	pthread_create(&id_t2, NULL, thread_summation, (void *)&range2);

	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);

	printf("result: %d\n", sum);
	return 0;
};

// gcc 18.thread3.c -D_REENTRANT -o 18.thread3 && ./18.thread3
