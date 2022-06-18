[bits 64]
LoadGDT:   
    lgdt [rdi]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    pop rdi
    mov rax, 0x08
    push rax
    push rdi
    retfq
GLOBAL LoadGDT

global olaf
olaf: 
    mov  rax,4     ; system call number (sys_write)
    mov  rbx,1     ; file descriptor (stdout)
    mov  rcx,0 ; message to write
    mov  rdx,12    ; message length
    int  0xCD      ; call kernel
    ret