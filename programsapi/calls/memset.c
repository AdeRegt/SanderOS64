#include <stdint.h>

void *memset(void *start, int value, long unsigned int num){
    for(upointer_t i = 0 ; i < num ; i++){
        *(unsigned char*)((upointer_t)start + i) = value;
    }
    return start;
}
