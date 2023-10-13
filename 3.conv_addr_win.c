#include <stdio.h>
#include <winsock2.h>

void ErrorHanding(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    char *strAddr = "203.211.218.102:9190";
    SOCKADDR_IN servAddr;
    int szAddr = sizeof(SOCKADDR_IN);
    WSAStringToAddress(strAddr, AF_INET, NULL, (SOCKADDR *)&servAddr, &szAddr);

    char strAddrBuf[50];
    DWORD szBuf = sizeof(strAddrBuf);
    WSAAddressToString((SOCKADDR *)&servAddr, szAddr, NULL, strAddrBuf, &szBuf);

    printf("Second conv result: %s\n", strAddrBuf);

    WSACleanup();

    return 0;
}