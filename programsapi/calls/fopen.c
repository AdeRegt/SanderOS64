#include <stdio.h>
#include <stdint.h>

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
