#include <stdio.h>

char buffer[2];

char getc(FILE* stream){
    int mode = 0;
    void *fp = stream;
    void *ptr = buffer;
    buffer[0] =0;
    buffer[1] =0;
    void* g = (void*)1;
	__asm__ __volatile__( "int $0x81" : "+a"(mode) , "+b" (fp), "+c" (g), "+d" (ptr));
    return buffer[0];
}