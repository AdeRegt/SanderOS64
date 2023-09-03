#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SanderOS.h>

#define WANTED_PORT 6667
#define WANTED_HOST "chat.freenode.net"

void onincommin(unsigned long address,unsigned long size){
    unsigned char *message = (unsigned char*) (address);
    printf("IRC: ");
    for(int i = 15; i < size ; i++){
        printf("%c",message[i]);
    }
    printf("\n");
}

int main(int argc,char** argv){
    uint8_t *targetip = get_ip_from_name(WANTED_HOST);
    printf("Chat: IP is %d.%d.%d.%d \n",targetip[0],targetip[1],targetip[2],targetip[3]);
    start_tcp_session(targetip,WANTED_PORT,onincommin);
    sleep(100);
    printf("Chat tcp session started\n");
    while(1){}
}