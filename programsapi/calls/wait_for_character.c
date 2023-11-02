unsigned char wait_for_character(){
    int mode = 404;
    unsigned char res[2];
	__asm__ __volatile__( "int $0x81" :  : "a"(mode) , "b" (res) );
    return res[0];
}