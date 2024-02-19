__attribute__((noreturn)) void exit(int number){
    int mode = 419;
	__asm__ __volatile__( "int $0x81" : "+a"(mode) , "+b" (number));
    for(;;);
}