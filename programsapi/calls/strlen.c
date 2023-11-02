#include <stdint.h>

upointer_t strlen(const char *ert){
    char len = 0;
    while(ert[len++]);
    len--;
    return len;
}