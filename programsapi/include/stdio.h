#include <stdint.h>

#define FILENAME_MAX 50
#define SEEK_SET 1
#define SEEK_END 2

#define EOF -1

typedef void FILE;
#define stdout 1
#define stderr 2

typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

#ifndef _pntr_end
    #define _pntr_end
    #ifdef __x86_64
        typedef uint64_t upointer_t;
    #else
        typedef uint32_t upointer_t;
    #endif
    typedef upointer_t off_t;
#endif 

int scanf( const char *format, ... );
int printf( const char *format, ... );
upointer_t fread( void *buffer, upointer_t size, upointer_t count, FILE *stream );
int fclose( FILE *stream );
long ftell( FILE *stream );
int fseek( FILE *stream, long offset, int origin );
FILE *fopen( const char *filename, const char *mode );
int vfprintf(FILE *stream, const char *format, va_list arg);
int sprintf(char *str, const char *string,...); 