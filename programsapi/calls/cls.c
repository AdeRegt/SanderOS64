void cls(){
    int mode = 414;
	__asm__ __volatile__( "int $0x81" : "+a"(mode));
}