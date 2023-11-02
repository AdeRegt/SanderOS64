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

int scanf( const char *format, ... ){
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