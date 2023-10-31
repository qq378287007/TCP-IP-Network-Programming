#include <stdio.h>
#include <windows.h>
#include <process.h>

#define STR_LEN 100

static char str[STR_LEN];
static HANDLE hEvent;

unsigned WINAPI NumberOfA(void *arg)
{
	int cnt = 0;
	WaitForSingleObject(hEvent, INFINITE); // 2
	for (int i = 0; str[i] != 0; i++)
		if (str[i] == 'A')
			cnt++;
	printf("Num of A: %d\n", cnt);
	return 0;
}

unsigned WINAPI NumberOfOthers(void *arg)
{
	int cnt = 0;
	WaitForSingleObject(hEvent, INFINITE); // 2
	for (int i = 0; str[i] != 0; i++)
		if (str[i] != 'A')
			cnt++;
	printf("Num of others: %d\n", cnt);
	return 0;
}

int main(int argc, char *argv[])
{
	// non-signaled状态创建manual-reset模式的事件对象
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	HANDLE id_t1 = (HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, 0);
	HANDLE id_t2 = (HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, 0);

	fputs("Input string: ", stdout);
	fgets(str, STR_LEN, stdin); // 0

	// 事件设置为signaled状态
	// id_t1和id_t2两个线程同时摆脱等待状态（manual-reset模式，事件对象会一直处于signaled状态）
	// 触发两个线程
	SetEvent(hEvent); // 1

	WaitForSingleObject(id_t1, INFINITE); // 3
	WaitForSingleObject(id_t2, INFINITE); // 4

	// 事件设置为not-signaled状态
	ResetEvent(hEvent);

	CloseHandle(hEvent);
	return 0;
};

// gcc 20.SyncEvent_win.c -o 20.SyncEvent_win && 20.SyncEvent_win
