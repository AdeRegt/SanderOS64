[ORG 0xC000000]
[BITS 64]

main:
    ; print hello world
    mov rax,4
    mov rbx,1
    mov rcx,message
    mov rdx,12
    int 0x80

    ; quit
    mov rax,1
    int 0x80

    ; just to make sure...
    cli
    hlt

message db "Hello world!",0x00