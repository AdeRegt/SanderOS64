#include <stdint.h>

#define FILENAME_MAX 50
#define SEEK_SET 1
#define SEEK_END 2

typedef void FILE;

int scanf( const char *format, ... );
int printf( const char *format, ... );
upointer_t fread( void *buffer, upointer_t size, upointer_t count, FILE *stream );
int fclose( FILE *stream );
long ftell( FILE *stream );
int fseek( FILE *stream, long offset, int origin );
FILE *fopen( const char *filename, const char *mode );