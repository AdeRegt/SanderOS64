#include <stdint.h>
upointer_t strlen( const char *str );
void* memcpy( void *dest, const void *src, upointer_t count );
void *memset(void *start, int value, long unsigned int num);
int strcmp ( const char * str1, const char * str2 );
long int strtol(const char *str, char **endptr, int base);
int memcmp(const void* aptr, const void* bptr, size_t n);