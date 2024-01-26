#include <stdio.h>
#include <stdint.h>

upointer_t fread( void *buffer, upointer_t size, upointer_t count, FILE *stream ){
    int modus = 0;
    upointer_t res = 0;
    __asm__ __volatile__( "int $0x81" : "=a" (res) : "a" (modus) , "b" (stream) , "d" (buffer) , "c" (count) );
    return res;
}
