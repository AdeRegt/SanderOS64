#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

__attribute__((noreturn)) void exit(int number){
    int mode = 1;
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (number));
    for(;;);
}

char buffer[2];

char getc(){
    int mode = 3;
    int fp = 1;
    int ptr = 1;
    buffer[0] =0;
    buffer[1] =0;
    void* g =buffer;
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (fp), "+c" (g), "+d" (ptr));
    return buffer[0];
}

long unsigned int strlen(const char *ert){
    char len = 0;
    while(ert[len++]);
    len--;
    return len;
}

int puts(const char *number){
    int mode = 4;
    int fp = 1;
    int ptr = strlen(number);
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (fp), "+c" (number), "+d" (ptr));
    return 0;
}

int putsc(const char p2){
    int mode = 4;
    int fp = 1;
    char number[2];
    number[0] = p2;
    number[1] = 0;
    char* tg = (char*)&number;
    int ptr = strlen(tg);
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (fp), "+c" (tg), "+d" (ptr));
    return 0;
}

char *convert(unsigned int num, int base) { 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 

    for(int i = 0 ; i < 50 ; i++){
        buffer[i] = 0x00;
    }

	if(num==0){
		ptr = &buffer[0];
		buffer[0] = '0';
		return ptr;
	}
	
	ptr = &buffer[49]; 
	*--ptr = '\0'; 
	*--ptr = '\0'; 
	*--ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0);
	
	return(ptr); 
}

typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

void hang(char* func){
    printf("\nHANG: %s \n",func);
    for(;;);
}

void scanline(char* where,char echo){
    int i = 0;
    while(1){
        char deze = getc();
        putsc(deze);
        if(deze=='\n'){
            break;
        }
        where[i++] = deze;
        where[i] = 0;
    }
}

int scanf( const char *format, ... ){
    // hang("scanf");
    va_list arg; 
	va_start(arg, format);
    int length = 0;
    while(1){
        char deze = format[length];
        if(deze=='\0'){
            break;
        }else if(deze=='%'){
            length++;
            deze = format[length];
            if(deze=='s'){
                char *s = va_arg(arg,char *);
                scanline(s,1);
            }else{
                hang("invalid thing to scan");
            }
        }else{
            putsc(deze);
        }
        length++;
    }
    va_end(arg);
}

int printf( const char* format,...){
    va_list arg; 
	va_start(arg, format);
    int length = 0;
    while(1){
        char deze = format[length];
        if(deze=='\0'){
            break;
        }else if(deze=='%'){
            length++;
            deze = format[length];
            if(deze=='c'){
                char i = va_arg(arg,int);
                putsc(i);
            }else if(deze=='%'){
                putsc('%');
            }else if(deze=='s'){
                char *s = va_arg(arg,char *);
                puts(s);
            }else if(deze=='x'){
                int t = va_arg(arg,unsigned int);
                putsc('0');
                putsc('x');
                char *convertednumber = convert(t,16);
                puts(convertednumber);
            }else if(deze=='d'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,10);
                puts(convertednumber);
            }else if(deze=='o'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,8);
                puts(convertednumber);
            }
            length++;
        }else{
            putsc(deze);
            length++;
        }
    }
    va_end(arg);
}

void* memcpy( void *dest, const void *src, uint64_t count ){
    for(uint64_t i = 0 ; i < count ; i++){
        *(unsigned char*)((uint64_t)dest + i) = *(unsigned char*)((uint64_t)src + i);
    }
}

int fclose( FILE *stream ){
    hang("fclose");
}

uint64_t fread( void *buffer, uint64_t size, uint64_t count, FILE *stream ){
    hang("fread");
}

int fseek( FILE *stream, long offset, int origin ){
    hang("fseek");
}

long ftell( FILE *stream ){
    hang("ftell");
}

FILE *fopen( const char *filename, const char *mode ){
    hang("fopen");
}

void* malloc( uint64_t size ){
    int mode = 400;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "d" (size) );
    return res;
}

void free( void* ptr ){
    hang("free");
}