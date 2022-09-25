#include <stdint.h>

void free( void* ptr );
void exit( int exit_code );
void* malloc( uint64_t size );
int system (const char* command);
void *calloc(long unsigned int nitems, long unsigned int size);

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1