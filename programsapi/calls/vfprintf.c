#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

int vfprintf(FILE *stream, const char *format, va_list arg){
    if(strlen(format)==0){
		return -1;
	}
    char* buffer = malloc(strlen(format));
	vsnprintf(buffer,0,format,arg);
    return write((int)((upointer_t)stream),buffer,strlen(buffer));
}
