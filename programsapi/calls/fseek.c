#include <stdio.h>
#include <stdint.h>

int fseek( FILE *stream, long int offset, int origin ){
    int modus = 8;
    int res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "b" (stream) , "c" (origin) , "d" ((int)offset) );
    return res;
}
