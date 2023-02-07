[bits 32]

global isrint
extern isrhandler
isrint:
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi 
    push edi
    
    mov edi,esp
    call isrhandler
    
    pop edi
    pop esi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    ;add rsp,16
    iret

global isr2int
extern isr2handler
isr2int:
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi 
    push edi
    
    mov edi,esp
    call isr2handler
    
    pop edi
    pop esi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    ;add rsp,16
    iret