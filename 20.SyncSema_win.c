#include <stdio.h>
#include <windows.h>
#include <process.h>

static HANDLE sem_one;
static HANDLE sem_two;
static int num;

unsigned WINAPI read(void *arg)
{
	for (int i = 0; i < 5; i++)
	{
		fputs("Input num: ", stdout);			// 0
		WaitForSingleObject(sem_two, INFINITE); // 1
		scanf("%d", &num);
		ReleaseSemaphore(sem_one, 1, NULL); // 2
	}
	return 0;
}

unsigned WINAPI accu(void *arg)
{
	int sum = 0;
	for (int i = 0; i < 5; i++)
	{
		WaitForSingleObject(sem_one, INFINITE); // 3
		sum += num;
		ReleaseSemaphore(sem_two, 1, NULL); // 4
	}
	printf("Result: %d\n", sum);
	return 0;
}

int main(int argc, char *argv[])
{
	sem_one = CreateSemaphore(NULL, 0, 1, NULL);
	sem_two = CreateSemaphore(NULL, 1, 1, NULL);

	HANDLE id_t1 = (HANDLE)_beginthreadex(NULL, 0, read, NULL, 0, 0);

	HANDLE id_t2 = (HANDLE)_beginthreadex(NULL, 0, accu, NULL, 0, 0);

	WaitForSingleObject(id_t1, INFINITE);
	WaitForSingleObject(id_t2, INFINITE);

	CloseHandle(sem_one);
	CloseHandle(sem_two);
	return 0;
};

// gcc 20.SyncSema_win.c -o 20.SyncSema_win && 20.SyncSema_win
