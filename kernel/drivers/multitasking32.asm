[bits 32]

global multitaskingint
extern multitaskinghandler
multitaskingint:
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi 
    push edi
    
    mov edi,esp
    call multitaskinghandler
    
    pop edi
    pop esi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    ;add rsp,16
    iret