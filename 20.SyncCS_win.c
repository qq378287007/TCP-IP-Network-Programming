#include <stdio.h>
#include <windows.h>
#include <process.h>

#define NUM_THREAD 50

long long num = 0;
CRITICAL_SECTION cs;

unsigned WINAPI thread_inc(void *arg)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < 50000000; i++)
		num += 1;
	LeaveCriticalSection(&cs);
	return 0;
}

unsigned WINAPI thread_des(void *arg)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < 50000000; i++)
		num -= 1;
	LeaveCriticalSection(&cs);
	return 0;
}

int main(int argc, char *argv[])
{
	InitializeCriticalSection(&cs);

	HANDLE thread_id[NUM_THREAD];
	for (int i = 0; i < NUM_THREAD; i++)
		if (i % 2)
			thread_id[i] = (HANDLE)_beginthreadex(NULL, 0, thread_inc, NULL, 0, NULL);
		else
			thread_id[i] = (HANDLE)_beginthreadex(NULL, 0, thread_des, NULL, 0, NULL);
	WaitForMultipleObjects(NUM_THREAD, thread_id, TRUE, INFINITE);

	DeleteCriticalSection(&cs);

	printf("result: %lld\n", num);
	return 0;
};