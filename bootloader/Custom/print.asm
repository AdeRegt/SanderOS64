; prints a string to the screen
; input: SI
bootloader_print_string:
; push all registers
pusha
.onceagain:
; load a new byte
mov al,byte [si]
; check if it is null, if yes, stop
cmp al,0
je .stop 
; otherwise print it
mov ah,0x0E
int 0x10
; increase our pointer
inc si
; and continue
jmp .onceagain
.stop:
; print a new line
mov al,13
int 0x10
mov al,10
int 0x10
; and return
popa
ret

bootloader_print_nibble:
; push all the registers
pusha 
; mask our data
and ax,0xF
; and call our printchar function
; compare if the character is more then 9, because otherwise we need to go to A,B etc
cmp ax,9
jle .isdidgit
; add offset
add ax, 'A'-'9'-1
.isdidgit:
; add a other offset here too
add ax, '0'
; now lets print all
mov ah,0x0E
int 0x10 
; pop all the registers
popa 
; return
ret

bootloader_print_byte:
; push all the registers
pusha 
; push al because we go shift it.
push ax 
; shift the nibble 
shr ax,4 
; now call our nibble printer
call bootloader_print_nibble
; pop ax back again
pop ax
; now call our nibble printer
call bootloader_print_nibble
; pop all the registers
popa 
; and return
ret

bootloader_print_word:
; push all the registers
pusha 
; push ax, because we go shift it
push ax 
; shift al with ah
mov al,ah 
; and call the byte print func 
call bootloader_print_byte
; pop ax back
pop ax
; and call the byte print func
call bootloader_print_byte
; pop all the registers
popa 
; and return
ret