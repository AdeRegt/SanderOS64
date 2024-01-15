BITS 16

bootloader_start:

    ; setup a stack
	mov ax, 07C0h
	add ax, 544

	cli
	mov ss, ax
	mov sp, 4096
	sti

	mov ax, 07C0h
	mov ds, ax

    ; set bootdevice 
    mov byte[bootdevice], dl

    ; print a welcomming message
    mov si,welcome_message
    call print_string

    ; reset bootdevice
	mov ax, 0
	mov dl, byte [bootdevice]
	stc
	int 13h
    jc error

    ; load all needed data

    mov dl, byte [bootdevice]
    mov ch, 0
    mov cl, 2
    mov dh, 0

	mov si, buffer
	mov bx, ds
	mov es, bx
	mov bx, si

	mov ah, 2
	mov al, 1

	stc
	int 13h
    jc error 

    call buffer
cli
hlt
welcome_message db "SanderOS64 Bootloader", 0x00
error_message db "Error :(",0x00
bootdevice db 0x00

error:
    mov si,error_message
    call print_string
    cli 
    hlt

print_string:
    pusha
    mov ah, 0Eh
    .repeat:
        lodsb
        cmp al, 0
        je .done
        int 10h
        jmp short .repeat
    .done:
        popa
        ret

; -----------------------------------------------------------------
; SECTOR INFORMATION

; times 0x1B8-($-$$) db 0
; db 0x3b, 0xbd, 0x87, 0xed, 0x00, 0x00, 0x00, 0x04,
; db 0x01, 0x04, 0x06, 0xfe, 0xc2, 0xff, 0x00, 0x08, 0x00, 0x00, 0x00, 0x48, 0x0f, 0x00, 0x00, 0x00,
; db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

; ------------------------------------------------------------------
; END OF BOOT SECTOR

times 510-($-$$) db 0
dw 0AA55h

buffer:
mov si,fffff
call print_string
cli 
hlt 
fffff db "DDDDDDDDDDDDDDDD",0x00
times 1024-($-$$) db 0