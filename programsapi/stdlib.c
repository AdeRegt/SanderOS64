#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <SanderOS.h>

typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

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

int vsprintf(char *str, const char *format, va_list arg)
{
    return vsnprintf(str,0,format,arg);
}