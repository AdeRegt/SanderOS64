#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <SanderOS.h>

void *get_ip_from_name(uint8_t *name){
    int mode = 405;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (name) );
    return res;
}

void *get_mac_from_ip(uint8_t *name){
    int mode = 406;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (name) );
    return res;
}

unsigned int sleep(unsigned int seconds){
    unsigned int modus = 408;
    int res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "b" (seconds) );
    return res;
}

char *itos(unsigned int num, int base) 
{ 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 
	
	ptr = &buffer[49]; 
	*ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0); 
	
	return(ptr); 
}

int isdigit(int c){
    return c > 47 && c < 58;
}