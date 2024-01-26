#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SanderOS.h>
#include <string.h>

// len 132
#define GET_DATA_1 "GET / HTTP/1.1\r\nHost: chat.sanderslando.nl\r\nUser-Agent: SanderOS64/1.0.0\r\nsnd_message: "
#define GET_DATA_2 "\r\nAccept: */*\r\n\r\n"
#define WANTED_PORT 80

static uint8_t messagedepot = 0;

void onincommin(unsigned long address,unsigned long size){
    if(size){
        unsigned char *message = (unsigned char*) (address);
        char tw = 0;
        for(int i = 12; i < size - 10 ; i++){
            if(message[i+0]=='\r'&&message[i+1]=='\n'&&message[i+2]=='\r'&&message[i+3]=='\n'){
                tw = 1;
                i += 3;
            }
            if(tw){
                printf("%c",message[i]);
            }
        }
        messagedepot = 2;
    }
    else if(address==1){
        printf("service started\n");
        messagedepot = 1;
    }
    else if(address==2){
        printf("service finished\n");
        messagedepot = 3;
    }
}

void wait_for_us(){
    messagedepot = 0;
    while(1){
        if( messagedepot == 1 || messagedepot == 2 || messagedepot == 3 ){
            break;
        }
    }
}

void wait_for(uint8_t wow){
    messagedepot = 0;
    while(1){
        if( messagedepot == wow ){
            break;
        }
    }
}

void sendMessage(char* buffer,uint8_t targetip[4]){
    char* normaldata_1 = GET_DATA_1 ;
    char* normaldata_2 = GET_DATA_2 ;
    int size = strlen(buffer);
    char sendbuffer[500];
    int zpointer = 0;
    for(int i = 0 ; i < strlen(normaldata_1) ; i++){
        sendbuffer[zpointer++] = normaldata_1[i];
    }
    for(int i = 0 ; i < size ; i++){
        sendbuffer[zpointer++] = buffer[i];
    }
    for(int i = 0 ; i < strlen(normaldata_2) ; i++){
        sendbuffer[zpointer++] = normaldata_2[i];
    }
    sendbuffer[zpointer++] = 0;
    sendbuffer[zpointer++] = 0;
    start_tcp_session(targetip,WANTED_PORT,onincommin);
    wait_for_us();
    send_tcp_message(targetip,WANTED_PORT,sendbuffer,strlen(sendbuffer));
    wait_for_us();
    wait_for_us();
}

int sendItem(uint8_t targetip[4]){
    printf("> ");
    char buffer[100];
    scanline((char*)&buffer);
    if(strlen((char*)&buffer)==0){
        return 1;
    }
    cls();
    sendMessage(buffer,targetip);
    printf("\n");
    return 0;
}

int main(){
    cls();
    uint8_t targetip[4];
    printf("Welcome to the chat!\n");
    targetip[0] = 185;
    targetip[1] = 104;
    targetip[2] = 29;
    targetip[3] = 22;
    printf("Chat: IP is %d.%d.%d.%d \n",targetip[0],targetip[1],targetip[2],targetip[3]);
    while(1){
        int t = sendItem(targetip);
        if(t){
            break;
        }
    }
    exit(1);
}