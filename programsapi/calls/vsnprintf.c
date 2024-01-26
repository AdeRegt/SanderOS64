#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

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
