all: crt064.o crt032.o stdlib64.o stdlib32.o 

crt064.o: crt064.S 
	$(AS) -c crt064.S -o crt064.o

crt032.o: crt032.S 
	$(AS) -c crt032.S -o crt032.o --32 

stdlib64.o: stdlib.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o stdlib64.o

stdlib32.o: stdlib.c 
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o stdlib32.o