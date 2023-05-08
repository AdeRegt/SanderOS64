#include <stdio.h>
#include <stdlib.h>
#include <SanderOS.h>

#define WANTED_PORT 6667
#define WANTED_HOST "chat.freenode.net"

void onincommin(unsigned long address,unsigned long size){
    printf("irc: %s \n",((char*)address));
}

int main(int argc,char** argv){
    uint8_t *targetip = get_ip_from_name(WANTED_HOST);
    printf("Chat: IP is %d.%d.%d.%d \n",targetip[0],targetip[1],targetip[2],targetip[3]);
    int handlerpointer = start_tcp_session(targetip,WANTED_PORT,onincommin);
    while(1);
}