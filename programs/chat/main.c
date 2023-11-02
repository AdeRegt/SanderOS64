#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SanderOS.h>
#include <string.h>

#define WANTED_PORT 6667
#define WANTED_HOST "chat.freenode.net"

#define IRC_JOIN "NICK sanderos64\r\nUSER sanderos64 0 * :realname\n"

static uint8_t messagedepot = 0;
#ifdef IRC_GET_HOSTIP
    uint8_t *targetip;
#else
    uint8_t targetip[4];
#endif

void onincommin(unsigned long address,unsigned long size){
    unsigned char *message = (unsigned char*) (address);
    printf("IRC: ");
    for(int i = 12; i < size-6 ; i++){
        printf("%c",message[i]);
    }
    printf("\n");
    messagedepot = 1;
}

int main(){
    printf("Welcome to the chat!\n");
    #ifdef IRC_GET_HOSTIP
        targetip = get_ip_from_name(WANTED_HOST);
    #else
        printf("IP lijst:\nA: freenode\nB: spotchat\nC: local\nD: gimp\n");
        char g = wait_for_character();
        if(g=='a'){
            targetip[0] = 149;
            targetip[1] = 28;
            targetip[2] = 246;
            targetip[3] = 185;
        }else if(g=='b'){
            targetip[0] = 178;
            targetip[1] = 63;
            targetip[2] = 97;
            targetip[3] = 96;
        }else if(g=='c'){
            targetip[0] = 192;
            targetip[1] = 168;
            targetip[2] = 2;
            targetip[3] = 4;
        }else if(g=='d'){
            targetip[0] = 130;
            targetip[1] = 239;
            targetip[2] = 18;
            targetip[3] = 219;
        }
    #endif 
    printf("Chat: IP is %d.%d.%d.%d \n",targetip[0],targetip[1],targetip[2],targetip[3]);
    start_tcp_session(targetip,WANTED_PORT,onincommin);
    while(1){
        if(messagedepot==1){
            break;
        }
    }

    send_tcp_message(targetip,WANTED_PORT,IRC_JOIN,strlen(IRC_JOIN));

    // sebdnessages();
    for(;;);exit(1);
}