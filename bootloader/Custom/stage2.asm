BITS 16

bootloader_entrypoint:
; set the stack
cli
mov ax, 0
mov ss, ax
mov sp, 0FFFFh
sti

cld

mov ax, 2000h			
mov ds, ax			
mov es, ax			
mov fs, ax			
mov gs, ax

; greet our nice new users
mov si,message 
call bootloader_print_string
call bootloader_halt

bootloader_halt:
cli 
hlt

%include "print.asm"


message db "Welcome to SanderOSx bootloader!",0x00