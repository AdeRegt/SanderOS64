#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <SanderOS.h>

uint8_t *scan_x_characters(upointer_t maxsize){
    int mode = 417;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "d" (maxsize) );
    return res;
}