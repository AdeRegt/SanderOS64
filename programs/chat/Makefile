all:
	nasm -felf32 startup.asm -o startup.o
	$(CC) -m32 -I "../../programsapi/include" -ffreestanding -fno-PIE -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c main.c -o ./mn.o -D __SANDEROSUSB
	$(CC) -m32 -ffreestanding -nostdlib -T ../../programsapi/linker32.ld -fno-PIE startup.o ./mn.o ../../programsapi/bin/stdlib32.o -o ../../bin/programs/32bit/chat.bin
	rm *.o