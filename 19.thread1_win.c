#include <stdio.h>
#include <windows.h>
#include <process.h> //_beginthreadex, _endthreadex

// WINAPI，Windows固有关键字，指定参数传递方向、分配的栈返回方式等函数调用相关规定。
unsigned WINAPI ThreadFunc(void *arg)
{
    int cnt = *((int *)arg);
    for (int i = 0; i < cnt; i++)
    {
        Sleep(1000);
        puts("running thread");
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int param = 5;
    unsigned threadID;
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void *)&param, 0, &threadID);
    if (hThread == 0)
    {
        puts("_beginthreadex() error");
        return -1;
    }

    Sleep(3000);

    puts("end of main");

    return 0;
}
