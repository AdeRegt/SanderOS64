#include <stdint.h>
#include <SanderOS.h>
#include <stdio.h>

void scanline(char* where){
    int modus = 413;
    __asm__ __volatile__( "int $0x81" :  : "a"(modus) , "b" (where) );
}
