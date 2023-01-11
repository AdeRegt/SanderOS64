[bits 32]

; thanks Fred Nora:
; https://github.com/frednora/gramado241/blob/main/landos/kernel/1pump/arch/x86/pumpcore/hw/hw.asm 
; https://github.com/frednora/gramado241/blob/main/landos/kernel/0mem/core/ps/disp/x86/x86cont.c
; https://github.com/frednora/gramado241/blob/main/landos/kernel/0mem/core/ps/disp/ts.c

global multitaskingint
extern multitaskinghandler
extern _context_ds
extern _context_es
extern _context_fs
extern _context_gs
extern _context_esi
extern _context_edi
extern _context_ebp
extern _context_ebx
extern _context_ecx
extern _context_edx
extern _context_eflags
extern _context_cs
extern _context_eip
extern _context_eax
multitaskingint:
    ; make sure interrupts do not happen when we are playing..
    cli
    ; we have a stackframe which we can recieve by popping the right values from the stack...
    pop dword [_context_eip]
    pop dword [_context_cs]
    pop dword [_context_eflags]
    ; save the general registers
    mov dword [_context_edx], edx
    mov dword [_context_ecx], ecx
    mov dword [_context_ebx], ebx
    mov dword [_context_eax], eax
    mov dword [_context_ebp], ebp
    mov dword [_context_edi], edi
    mov dword [_context_esi], esi
    ; save the segments registers
    mov eax,0
    mov ax, gs 
    mov word [_context_gs], gs
    mov ax, fs 
    mov word [_context_fs], fs
    mov ax, es 
    mov word [_context_es], es
    mov ax, ds 
    mov word [_context_ds], ds
    ; set the right segment register for the current state
    ; this is also to switch to kernel mode
    mov eax, 0
    mov ax, word 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov eax, 0x003FFFF0 
    mov esp, eax
    ; call the functions
    ; call multitaskinghandler
    ; now restore the segment registers
    mov eax,0
    mov ax, word [_context_ds]
    mov ds, ax
    mov ax, word [_context_es]
    mov es, ax
    mov ax, word [_context_fs]
    mov fs, ax
    mov ax, word [_context_gs]
    mov gs, ax
    ; now restore the general registers
    mov esi, dword [_context_esi]
    mov edi, dword [_context_edi]
    mov ebp, dword [_context_ebp]
    mov eax, dword [_context_eax]
    mov ebx, dword [_context_ebx]
    mov ecx, dword [_context_ecx]
    mov edx, dword [_context_edx]
    ; now restore the stackframe
    pop dword [_context_eflags]
    pop dword [_context_cs]
    pop dword [_context_eip]
    ; tell the interrupts we are done with them
    mov al, 0x20
    out 0x20, al 
    ; wait a bit 
    ; now restore the ax register
    mov eax, dword [_context_eax]
    ; now, restore the context
    iret

global gdt_flush     ; Allows the C code to link to this
extern gp            ; Says that '_gp' is in another file
gdt_flush:
    lgdt [gp]        ; Load the GDT with our '_gp' which is a special pointer
    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2   ; 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               ; Returns back to the C code!