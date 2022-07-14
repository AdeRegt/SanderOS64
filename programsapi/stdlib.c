#include "SanderOS.h"

__attribute__((noreturn)) void exit(int number){
    int mode = 1;
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (number));
    for(;;);
}

char buffer[2];

char getc(){
    int mode = 3;
    int fp = 1;
    int ptr = 1;
    buffer[0] =0;
    buffer[1] =0;
    void* g =buffer;
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (fp), "+c" (g), "+d" (ptr));
    return buffer[0];
}

long unsigned int strlen(const char *ert){
    char len = 0;
    while(ert[len++]);
    len--;
    return len;
}

int puts(const char *number){
    int mode = 4;
    int fp = 1;
    int ptr = strlen(number);
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (fp), "+c" (number), "+d" (ptr));
    return 0;
}