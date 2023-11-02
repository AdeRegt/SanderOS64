#include <stdint.h>
#include <SanderOS.h>

void *send_tcp_message(uint8_t *ip,uint16_t port,void *buffer,int size){
    int mode = 412;
    void* res = 0;
    PackageRecievedDescriptor pest;
    pest.low_buf = buffer;
    pest.buffersize = size;
    void* functionp = (void*) &pest;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (ip), "c" (port), "d" (functionp) );
    return res;
}