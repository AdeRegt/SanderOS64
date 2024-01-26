#include <stdio.h>
#include <stdint.h>

upointer_t fwrite(void *ptr, upointer_t size, upointer_t nmemb, FILE *stream){
    int modus = 1;
    upointer_t res = 0;
    __asm__ __volatile__( "int $0x81" : "=a" (res) : "a" (modus) , "b" (stream) , "d" (ptr) , "c" (nmemb) );
    return res;
}
