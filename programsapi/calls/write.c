#include <stdio.h>

uint32_t write(int fildes, void *buf, uint32_t nbyte){
    int mode = 4;
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (fildes), "+c" (buf), "+d" (nbyte));
    return 0;
}
