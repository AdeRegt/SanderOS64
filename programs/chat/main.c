#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SanderOS.h>

#define WANTED_PORT 6667
#define WANTED_HOST "chat.freenode.net"

static uint8_t messagedepot = 0;

void onincommin(unsigned long address,unsigned long size){
    unsigned char *message = (unsigned char*) (address);
    printf("IRC: ");
    for(int i = 12; i < size-5 ; i++){
        printf("%c",message[i]);
    }
    printf("\n");
    messagedepot = 1;
}

int main(){
    printf("Welcome to the chat!\n");
    #ifdef IRC_GET_HOSTIP
        uint8_t *targetip = get_ip_from_name(WANTED_HOST);
    #else
        uint8_t targetip[4];
        #ifdef IRC_STATIC_FREENODE
            targetip[0] = 149;
            targetip[1] = 28;
            targetip[2] = 246;
            targetip[3] = 185;
        #else 
            targetip[0] = 192;
            targetip[1] = 168;
            targetip[2] = 2;
            targetip[3] = 4;
        #endif 
    #endif 
    printf("Chat: IP is %d.%d.%d.%d \n",targetip[0],targetip[1],targetip[2],targetip[3]);
    start_tcp_session(targetip,WANTED_PORT,onincommin);
    while(1){
        if(messagedepot==1){
            break;
        }
    }
    send_tcp_message(targetip,WANTED_PORT,"onincommin\n",11);
    for(;;);
}