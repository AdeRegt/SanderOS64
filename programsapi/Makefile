all: crt064.o stdlib64.o stdlib32.o 

crt064.o: crt064.S 
	$(AS) -c crt064.S -o crt064.o

stdlib64.o: stdlib.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o stdlib64.o

stdlib32.o: stdlib.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o stdlib32.o