#include <stdint.h> 
#include <string.h>

char *strncpy(char *dest, const char *src, size_t n){
    size_t size = strlen (src);
    if (size != n){
        memset (dest + size, '\0', n - size);
    }
    return memcpy (dest, src, size);
}
