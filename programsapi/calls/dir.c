#include <SanderOS.h>

char* dir(char* basepath){
    int mode = 415;
    char* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "d" (basepath) );
    return res;
}