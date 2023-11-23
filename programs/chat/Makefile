all:
	nasm -felf32 startup.asm -o startup.o
	$(CC) -m32 -I "../../programsapi/include" -ffreestanding -fno-PIE -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -c main.c -o ./mn.o -D __SANDEROSUSB
	$(CC) -m32 -ffreestanding -nostdlib -T ../../programsapi/linker32.ld -fno-PIE startup.o ./mn.o ../../programsapi/bin/exit32.o ../../programsapi/bin/cls32.o ../../programsapi/bin/scanline32.o ../../programsapi/bin/start_tcp_session32.o ../../programsapi/bin/send_tcp_message32.o ../../programsapi/bin/hang32.o ../../programsapi/bin/convert32.o ../../programsapi/bin/puts32.o ../../programsapi/bin/putsc32.o ../../programsapi/bin/__stack_chk_fail32.o ../../programsapi/bin/printf32.o ../../programsapi/bin/strlen32.o -o ../../bin/programs/32bit/chat.bin
	rm *.o