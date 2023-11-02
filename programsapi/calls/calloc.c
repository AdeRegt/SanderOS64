#include <stdint.h>

void* calloc( upointer_t size ){
    int mode = 400;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "d" (size) );
    for(upointer_t i = 0 ; i < size ; i++){
        ((uint8_t*)res)[i] = 0;
    }
    return res;
}
