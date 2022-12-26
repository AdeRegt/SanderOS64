all: crt0.o stdlib.o 

crt0.o: crt0.S 
	$(AS) -c crt0.S -o crt0.o

stdlib.o: stdlib.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o stdlib.o