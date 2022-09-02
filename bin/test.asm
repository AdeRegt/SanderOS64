format elf64 ;binary
use64

org 0xC000000

main:
mov rax,1
mov rdi,1
mov rdx,6
mov rsi,modi
int 0x81

mov rax,60
int 0x81

modi db "hello",0x0a