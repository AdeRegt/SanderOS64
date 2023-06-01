[ORG 0xC000000]
[BITS 32]

main:
    ; print hello world
    mov eax,4
    mov ebx,1
    mov ecx,message
    mov edx,12
    int 0x80

    ; quit
    mov eax,1
    int 0x80

    ; just to make sure...
    cli
    hlt

message db "Hello world!",0x00