#include <SanderOS.h>

int exec(uint8_t *path,char *argv){
    int mode = 418;
	__asm__ __volatile__( "int $0x81" : "+a"(mode) , "+b" (path), "+c" (argv));
    return 0;
}