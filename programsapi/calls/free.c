
void free( void* ptr ){
    int mode = 409;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (ptr) );
}
