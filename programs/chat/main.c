#include <stdio.h>
#include <string.h>
#include <SanderOS.h>
#include <stdint.h>

#define PORT 6667

INetRequest dat;

static int isworking = 1;
static uint8_t buffer[200];

void chat_onchat(void *addr,uint32_t count){
    printf("< ");
    for(uint32_t i = 0 ; i < count ; i++){
        printf("%c",((uint8_t*)addr)[i]);
    }
    printf("\n");
    isworking = 0;
}

int main(int argc, char const* argv)
{
    printf("Welcome to chat!\n");
    #ifdef trylocal
    unsigned char prefip[4] = {192,168,2,3};
    #else 
    unsigned char prefip[4] = {130,185,232,126};
    #endif 
    memcpy(dat.address,prefip,4);
    dat.port = PORT;
    dat.function = chat_onchat;
    setup_tcp_connection((INetRequest*)&dat);

    while(isworking);

    char* msg = "Hello from SanderOS!\n";
    send_tcp_message(PORT,msg,strlen(msg));

    return 0xCD;
}
