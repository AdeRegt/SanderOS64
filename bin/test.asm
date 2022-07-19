format BIN at 0xC000000

mov rax,0
int 0x80

hw db "Hello world!",0