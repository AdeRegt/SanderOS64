all:
	nasm -felf32 startup.asm -o startup32.o
	nasm -felf64 startup.asm -o startup64.o
	$(CC) -m32 -I "../../programsapi/include" -ffreestanding -fno-PIE -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c main.c -o ./mn32.o -D __SANDEROSUSB
	$(CC) -m64 -I "../../programsapi/include" -ffreestanding -fno-PIE -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c main.c -o ./mn64.o -D __SANDEROSUSB
	$(CC) -m32 -ffreestanding -nostdlib -T ../../programsapi/linker32.ld -fno-PIE ./startup32.o ../../programsapi/bin/hang32.o ../../programsapi/bin/__stack_chk_fail32.o ../../programsapi/bin/convert32.o ../../programsapi/bin/putsc32.o ../../programsapi/bin/strlen32.o ../../programsapi/bin/puts32.o ../../programsapi/bin/printf32.o ../../programsapi/bin/exit32.o ./mn32.o -o ./args32.bin
	$(CC) -m64 -ffreestanding -nostdlib -T ../../programsapi/linker64.ld -fno-PIE ./startup64.o ../../programsapi/bin/hang64.o ../../programsapi/bin/__stack_chk_fail64.o ../../programsapi/bin/convert64.o ../../programsapi/bin/putsc64.o ../../programsapi/bin/strlen64.o ../../programsapi/bin/puts64.o ../../programsapi/bin/printf64.o ../../programsapi/bin/exit64.o ./mn64.o -o ./args64.bin
	rm *.o
	cp ./args32.bin ../../bin/programs/32bit/args.bin
	cp ./args64.bin ../../bin/programs/64bit/args.bin