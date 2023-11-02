#include <stdint.h>

uint32_t gettimeofday(void *buff){
    unsigned int modus = 96;
    int res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "d" (buff) );
    return res;
}
