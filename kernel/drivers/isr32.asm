[bits 32]
extern _inti_ds
extern _inti_es
extern _inti_fs
extern _inti_gs
extern _inti_esi
extern _inti_edi
extern _inti_ebp
extern _inti_ebx
extern _inti_ecx
extern _inti_edx
extern _inti_eflags
extern _inti_cs
extern _inti_eip
extern _inti_eax

global isrint
extern isrhandler
isrint:
    ; make sure interrupts do not happen when we are playing..
    cli

    ; we have a stackframe which we can recieve by popping the right values from the stack...
    pop dword [_inti_eip]
    pop dword [_inti_cs]
    pop dword [_inti_eflags]

    ; save the general registers
    mov dword [_inti_edx], edx
    mov dword [_inti_ecx], ecx
    mov dword [_inti_ebx], ebx
    mov dword [_inti_eax], eax
    mov dword [_inti_ebp], ebp
    mov dword [_inti_edi], edi
    mov dword [_inti_esi], esi

    ; save the segments registers
    mov eax,0
    mov ax, gs 
    mov word [_inti_gs], gs
    mov ax, fs 
    mov word [_inti_fs], fs
    mov ax, es 
    mov word [_inti_es], es
    mov ax, ds 
    mov word [_inti_ds], ds

    ; set the right segment register for the current state
    ; this is also to switch to kernel mode
    mov eax, 0
    mov ax, word 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov eax, __new_stack_end 
    mov esp, eax

    pushad
    call isrhandler
    popad

    mov eax,0
    mov ax, word [_inti_ds]
    mov ds, ax
    mov ax, word [_inti_es]
    mov es, ax
    mov ax, word [_inti_fs]
    mov fs, ax
    mov ax, word [_inti_gs]
    mov gs, ax

    ; now restore the general registers
    mov esi, dword [_inti_esi]
    mov edi, dword [_inti_edi]
    mov ebp, dword [_inti_ebp]
    mov eax, dword [_inti_eax]
    mov ebx, dword [_inti_ebx]
    mov ecx, dword [_inti_ecx]
    mov edx, dword [_inti_edx]

    ; now restore the stackframe
    push dword [_inti_eflags]
    push dword [_inti_cs]
    push dword [_inti_eip]

    ; tell the interrupts we are done with them
    mov al, 0x20
    out 0x20, al 

    ; wait a bit 
    ; now restore the ax register
    mov eax, dword [_inti_eax]
    ; now, restore the inti
    sti
    iretd

global isr2int
extern isr2handler
isr2int:
    ; make sure interrupts do not happen when we are playing..
    cli

    ; we have a stackframe which we can recieve by popping the right values from the stack...
    pop dword [_inti_eip]
    pop dword [_inti_cs]
    pop dword [_inti_eflags]

    ; save the general registers
    mov dword [_inti_edx], edx
    mov dword [_inti_ecx], ecx
    mov dword [_inti_ebx], ebx
    mov dword [_inti_eax], eax
    mov dword [_inti_ebp], ebp
    mov dword [_inti_edi], edi
    mov dword [_inti_esi], esi

    ; save the segments registers
    mov eax,0
    mov ax, gs 
    mov word [_inti_gs], gs
    mov ax, fs 
    mov word [_inti_fs], fs
    mov ax, es 
    mov word [_inti_es], es
    mov ax, ds 
    mov word [_inti_ds], ds

    ; set the right segment register for the current state
    ; this is also to switch to kernel mode
    mov eax, 0
    mov ax, word 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov eax, __new_stack_end 
    mov esp, eax

    pushad
    call isr2handler
    popad

    mov eax,0
    mov ax, word [_inti_ds]
    mov ds, ax
    mov ax, word [_inti_es]
    mov es, ax
    mov ax, word [_inti_fs]
    mov fs, ax
    mov ax, word [_inti_gs]
    mov gs, ax

    ; now restore the general registers
    mov esi, dword [_inti_esi]
    mov edi, dword [_inti_edi]
    mov ebp, dword [_inti_ebp]
    mov eax, dword [_inti_eax]
    mov ebx, dword [_inti_ebx]
    mov ecx, dword [_inti_ecx]
    mov edx, dword [_inti_edx]

    ; now restore the stackframe
    push dword [_inti_eflags]
    push dword [_inti_cs]
    push dword [_inti_eip]

    ; tell the interrupts we are done with them
    mov al, 0x20
    out 0x20, al 

    ; wait a bit 
    ; now restore the ax register
    mov eax, dword [_inti_eax]
    ; now, restore the inti
    sti
    iretd