#include <stdio.h>

typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

int sprintf(char *str, const char *format, ...){
    va_list arg; 
	va_start(arg, format);
    int t = vsnprintf(str,0,format,arg);
	va_end(arg);
    return t;
}
