#include <stdint.h>

void free( void* ptr );
void exit( int exit_code );
void* malloc( upointer_t size );

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1