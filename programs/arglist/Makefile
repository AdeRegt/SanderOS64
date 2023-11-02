all:
	nasm -felf32 startup.asm -o startup.o
	$(CC) -m32 -I "../../programsapi/include" -ffreestanding -fno-PIE -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c main.c -o ./mn.o -D __SANDEROSUSB
	$(CC) -m32 -ffreestanding -nostdlib -T ../../programsapi/linker32.ld -fno-PIE ./startup.o ../../programsapi/bin/hang32.o ../../programsapi/bin/__stack_chk_fail32.o ../../programsapi/bin/convert32.o ../../programsapi/bin/putsc32.o ../../programsapi/bin/strlen32.o ../../programsapi/bin/puts32.o ../../programsapi/bin/printf32.o ../../programsapi/bin/exit32.o ./mn.o -o ./args.bin
	rm *.o
	cp ./args.bin ../../bin/programs/32bit/args.bin