typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

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