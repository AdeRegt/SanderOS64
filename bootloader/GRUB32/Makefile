all:
	nasm -felf32 boot.asm -o boot.o || exit
	gcc -m32 -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O0 -Wall -Wextra || exit
	gcc -T linker.ld -o bootloader.bin  -m32 -O2 -ffreestanding -nostdlib boot.o kernel.o || exit
	cp bootloader.bin ../../bin/sanderos/bootloader.bin 
	rm *.o