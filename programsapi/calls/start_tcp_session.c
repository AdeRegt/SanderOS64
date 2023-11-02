#include <stdint.h>
#include <SanderOS.h>

void *start_tcp_session(uint8_t *ip,uint16_t port,void *functionp){
    int mode = 407;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (ip), "c" (port), "d" (functionp) );
    return res;
}