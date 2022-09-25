[bits 64]
[global start]
[extern main]

start:
    mov rax,405
    int 0x81

    push rbp

    call main 

    mov rax,60
    int 0x81