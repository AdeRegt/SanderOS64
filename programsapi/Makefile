all: bin/crt064.o bin/crt032.o bin/stdlib64.o bin/stdlib32.o 

bin/crt064.o: crt064.S 
	$(AS) -c crt064.S -o bin/crt064.o

bin/crt032.o: crt032.S 
	$(AS) -c crt032.S -o bin/crt032.o --32 

bin/stdlib64.o: stdlib.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o bin/stdlib64.o

bin/stdlib32.o: stdlib.c 
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o bin/stdlib32.o