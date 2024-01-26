#include <stdint.h>

void* memcpy( void *dest, const void *src, upointer_t count ){
    for(upointer_t i = 0 ; i < count ; i++){
        ((uint8_t*)(dest+i))[0] = ((uint8_t*)(src+i))[0];
    }
}