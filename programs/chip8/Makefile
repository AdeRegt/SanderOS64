all:
	nasm -felf64 startup.asm -o startup.o
	$(CC) -I "../../programsapi/include" -ffreestanding -fno-PIE -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c origin/main.c -o ./chip8.o -D __SANDEROSUSB
	$(CC) -ffreestanding -nostdlib -lgcc -T ./linker.ld -fno-PIE ./startup.o ../../programsapi/stdlib64.o ./chip8.o -o ./chip8.bin
	rm *.o
	cp ./chip8.bin ../../bin/programs/64bit/chip8.bin