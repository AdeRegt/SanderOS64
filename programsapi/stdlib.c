#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <SanderOS.h>

__attribute__((noreturn)) void exit(int number){
    int mode = 1;
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (number));
    for(;;);
}

char buffer[2];

char getc(FILE* stream){
    int mode = 0;
    void *fp = stream;
    void *ptr = buffer;
    buffer[0] =0;
    buffer[1] =0;
    void* g = (void*)1;
	__asm__ __volatile__( "int $0x81" : "+a"(mode) , "+b" (fp), "+c" (g), "+d" (ptr));
    return buffer[0];
}

upointer_t strlen(const char *ert){
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

void hang(const char* func){
    printf("\nHANG: %s \n",func);
    for(;;);
}

void scanline(char* where,char echo){
    int i = 0;
    while(1){
        char deze = scan_for_character();//getc(stdin);
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
            }else if(deze=='d'||deze=='i'){
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

void* memcpy( void *dest, const void *src, upointer_t count ){
    for(upointer_t i = 0 ; i < count ; i++){
        ((uint8_t*)(dest+i))[0] = ((uint8_t*)(src+i))[0];
    }
}

int fclose( FILE *stream ){
    int modus = 3;
    upointer_t res = 0;
    __asm__ __volatile__( "int $0x81" : "=a" (res) : "a" (modus) , "D" (stream) );
    return res;
}

upointer_t fread( void *buffer, upointer_t size, upointer_t count, FILE *stream ){
    int modus = 0;
    upointer_t res = 0;
    __asm__ __volatile__( "int $0x81" : "=a" (res) : "a" (modus) , "b" (stream) , "d" (buffer) , "c" (count) );
    return res;
}

int fseek( FILE *stream, long int offset, int origin ){
    int modus = 8;
    int res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "b" (stream) , "c" (origin) , "d" ((int)offset) );
    return res;
}

long ftell( FILE *stream ){
    int modus = 411;
    int res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "b" (stream) );
    return res;
}

FILE *fopen( const char *filename, const char *mode ){
    int modus = 2;
    int enos = mode[0]=='w'?1:0;
    void* res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "d" (filename) , "S" (enos) );
    if((upointer_t)res==-1){
        return 0;
    }
    return res;
}

void* malloc( upointer_t size ){
    int mode = 400;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "d" (size) );
    return res;
}

void free( void* ptr ){
    int mode = 409;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (ptr) );
}

void *draw_pixel(int x,int y,int z){
    int mode = 402;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (x) , "c" (y) , "d" (z) );
    return res;
}

void *memset(void *start, int value, long unsigned int num){
    for(upointer_t i = 0 ; i < num ; i++){
        *(unsigned char*)((upointer_t)start + i) = value;
    }
    return start;
}

unsigned char scan_for_character(){
    int mode = 403;
    unsigned char res[2];
	__asm__ __volatile__( "int $0x81" :  : "a"(mode) , "b" (res) );
    return res[0];
}

unsigned char wait_for_character(){
    int mode = 404;
    unsigned char res[2];
	__asm__ __volatile__( "int $0x81" :  : "a"(mode) , "b" (res) );
    return res[0];
}

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

void *start_tcp_session(uint8_t *ip,uint16_t port,void *functionp){
    int mode = 407;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (ip), "c" (port), "d" (functionp) );
    return res;
}

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

unsigned int sleep(unsigned int seconds){
    unsigned int modus = 408;
    int res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "b" (seconds) );
    return res;
}

void __stack_chk_fail(){
    hang(__FUNCTION__);
    for(;;);
}

char *getenv(char *name){
    int modus = 410;
    int enos = 0;
    void* res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "d" (name) );
    return res;
}

upointer_t fwrite(void *ptr, upointer_t size, upointer_t nmemb, FILE *stream){
    int modus = 1;
    upointer_t res = 0;
    __asm__ __volatile__( "int $0x81" : "=a" (res) : "a" (modus) , "b" (stream) , "d" (ptr) , "c" (nmemb) );
    return res;
}

uint32_t time(void *t){
    hang(__FUNCTION__);
    return 1;
}

uint32_t write(int fildes, void *buf, uint32_t nbyte){
    int mode = 4;
	__asm__ __volatile__( "int $0x80" : "+a"(mode) , "+b" (fildes), "+c" (buf), "+d" (nbyte));
    return 0;
}

uint32_t gettimeofday(void *buff){
    unsigned int modus = 96;
    int res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "d" (buff) );
    return res;
}

int strcmp ( const char * a, const char * b ){
    register const unsigned char *s1 = (const unsigned char *) a;
    register const unsigned char *s2 = (const unsigned char *) b;
    unsigned char reg_char; 
    unsigned char c1; 
    unsigned char c2;

    do{
        c1 = (unsigned char) *s1++;
        c2 = (unsigned char) *s2++;
        if (c1 == '\0'){
            return c1 - c2;
        }
    }while (c1 == c2);

    return c1 - c2;
}

void assert(int a){
    if(!a){
        printf("__ASSERT FAILURE__");
        hang("");
    }
}

int system (const char* command){
    return -1;
}

void* calloc( upointer_t size ){
    int mode = 400;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "d" (size) );
    for(upointer_t i = 0 ; i < size ; i++){
        ((uint8_t*)res)[i] = 0;
    }
    return res;
}

int sprintf(char *str, const char *format, ...){
    va_list arg; 
	va_start(arg, format);
    int t = vsnprintf(str,0,format,arg);
	va_end(arg);
    return t;
}

int vsnprintf(char *buffer, size_t size, const char *format, va_list arg){
    if(strlen(format)==0){
		return -1;
	}
	size_t travelpointer = 0;
    size_t length = 0;
	while(1){
        char deze = format[length];
        if(deze=='\0'){
            break;
        }else if(deze=='%'){
            length++;
            deze = format[length];
            if(deze=='c'){
                char i = va_arg(arg,int);
                buffer[travelpointer++] = i;
            }else if(deze=='%'){
                buffer[travelpointer++] = '%';
            }else if(deze=='s'){
                char *s = va_arg(arg,char *);
                int tz = strlen(s);
                for(int tv = 0 ; tv < tz ; tv++){
                    buffer[travelpointer++] = s[tv];
                }
            }else if(deze=='x'){
                int t = va_arg(arg,unsigned int);
                buffer[travelpointer++] = '0';
                buffer[travelpointer++] = 'x';
                char *convertednumber = convert(t,16);
                int tz = strlen(convertednumber);
                for(int tv = 0 ; tv < tz ; tv++){
                    buffer[travelpointer++] = convertednumber[tv];
                }
            }else if(deze=='d'||deze=='i'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,10);
                int tz = strlen(convertednumber);
                for(int tv = 0 ; tv < tz ; tv++){
                    buffer[travelpointer++] = convertednumber[tv];
                }
            }else if(deze=='o'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,8);
                int tz = strlen(convertednumber);
                for(int tv = 0 ; tv < tz ; tv++){
                    buffer[travelpointer++] = convertednumber[tv];
                }
            }
            length++;
        }else{
            buffer[travelpointer++] = deze;
            length++;
        }
    }
    // memcpy(str,buffer,travelpointer);
    return travelpointer;
}

int vfprintf(FILE *stream, const char *format, va_list arg){
    if(strlen(format)==0){
		return -1;
	}
    char* buffer = malloc(strlen(format));
	vsnprintf(buffer,0,format,arg);
    return write((int)((upointer_t)stream),buffer,strlen(buffer));
}

int fprintf(FILE *stream, char *format, ...){
	va_list arg; 
	va_start(arg, format);
    int t = vfprintf(stream,format,arg);
	va_end(arg); 
    return t;
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

int tolower(int c){
    return c+32;
}

long int strtol(const char *str, char **endptr, int base){
    long int result = 0;
	int pointer = strlen(str)-1;
	int min = -1;
	if(base==16){
		for(size_t i = 0 ; i < strlen(str) ; i++){
			if(str[i]=='x'){
				min = i;
			}
		}
	}
	int power = 1;
	for(int i = pointer ; i > min ; i--){
		char deze = str[i];
		if(base==10){
			if(deze>='0'&&deze<='9'){
				char t = deze-'0';
				result += (t*power);
			}
			power *= 10;
		}else if(base==16){
			int t = 0;
			if(deze>='0'&&deze<='9'){
				t = deze-'0';
			}else if(deze>='A'&&deze<='Z'){
				t = 10+(deze-'A');
			}else if(deze>='a'&&deze<='z'){
				t = 10+(deze-'a');
			}
			result += t*(16^(power-1));
			power++;
		}
	}
	*endptr = (char*) str;
	return result;
}

char *strncpy(char *dest, const char *src, size_t n){
    size_t size = strlen (src);
    if (size != n){
        memset (dest + size, '\0', n - size);
    }
    return memcpy (dest, src, size);
}

int isalpha(int c){
    return c > 64 && c < 123;
}

long long atoll(const char *nptr){
    return strtol (nptr, (char **) NULL, 10);
}

int isdigit(int c){
    return c > 47 && c < 58;
}

int fputc(int chart, FILE *stream){
    char t[2];
    t[0] = 0;
    t[1] = 0;
    t[0] = chart;
    write((int)((upointer_t)stream),t,1);
}

int ungetc(int chart, FILE *stream){
    fputc(chart,stream);
}

char *realpath(const char *restrict path, char *restrict resolved_path)
{
    return NULL;
}

int putchar(int chart){
    return putsc(chart);
}

void *realloc(void *ptr, size_t size){
    return ptr;
}

char* strcpy(char* destination, const char* source){
    int size = strlen(source);
    memcpy(destination,source,size);
    return destination;
}

int vsprintf(char *str, const char *format, va_list arg)
{
    return vsnprintf(str,0,format,arg);
}