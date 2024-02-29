;
; This is the bootloader of the SanderOS64 kernel!
; Thanks for the inspiration:
; https://github.com/gramado/kernel/blob/main/boot/vd/fat/main.asm
; https://github.com/nelsoncole/sirius-x86-64/blob/main/boot/stage0/stage0.asm
; https://github.com/mig-hub/mikeOS/blob/master/source/bootload/bootload.asm 
;

[BITS 16]
[ORG 0]

%include "fat16_signature.asm"

bootdev db 0x00

bootloader_start:
    cli
    mov ax, 0x07C0
    mov ds, ax
    mov es, ax
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0x6000
    sti
    mov byte [bootdev], dl

    jmp start2:0

start2:

%include "bootsignature.asm"

buffer: