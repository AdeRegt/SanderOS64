#include <stdio.h>
#include <stdint.h>

int fclose( FILE *stream ){
    int modus = 3;
    upointer_t res = 0;
    __asm__ __volatile__( "int $0x81" : "=a" (res) : "a" (modus) , "D" (stream) );
    return res;
}
