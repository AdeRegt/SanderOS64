all:
	$(CC) -m32 -I "../../programsapi/include" -ffreestanding -fno-PIE -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c main.c -o ./mn.o -D __SANDEROSUSB
	$(CC) -m32 -ffreestanding -nostdlib -T ../../programsapi/linker32.ld -fno-PIE ../../programsapi/stdlib32.o ../../programsapi/crt032.o ./mn.o -o ./chat.bin
	rm *.o
	cp ./chat.bin ../../bin/programs/32bit/chat.bin