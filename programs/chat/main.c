#include <stdio.h>
#include <string.h>
#include <SanderOS.h>

#define PORT 6667

INetRequest dat;

int main(int argc, char const* argv)
{
    printf("Welcome to chat!\n");
    setup_tcp_connection((INetRequest*)&dat);

    return 0xCD;
}
