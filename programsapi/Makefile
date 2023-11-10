all: bin/crt064.o bin/system32.o bin/cls32.o bin/tolower32.o bin/atoll32.o bin/isalpha32.o bin/fputc32.o bin/vsprintf32.o bin/strtol32.o bin/realloc32.o bin/vsnprintf32.o bin/vfprintf32.o bin/fprintf32.o bin/sprintf32.o bin/putchar32.o bin/realpath32.o bin/ungetc32.o bin/strncpy32.o bin/calloc32.o bin/strcmp32.o bin/assert32.o bin/crt032.o bin/gettimeofday32.o bin/getenv32.o bin/fwrite32.o bin/stdlib64.o bin/write32.o bin/time32.o bin/strcpy32.o bin/scanline32.o bin/free32.o bin/memset32.o bin/malloc32.o bin/fopen32.o bin/ftell32.o bin/fseek32.o bin/stdlib32.o bin/fread32.o bin/fclose32.o bin/draw_pixel32.o bin/memcpy32.o bin/scanf32.o bin/scan_for_character32.o bin/exit32.o bin/wait_for_character32.o bin/getc32.o bin/strlen32.o bin/printf32.o bin/puts32.o bin/putsc32.o bin/convert32.o bin/__stack_chk_fail32.o bin/hang32.o bin/send_tcp_message32.o bin/start_tcp_session32.o

bin/crt064.o: crt064.S 
	$(AS) -c crt064.S -o bin/crt064.o

bin/crt032.o: crt032.S 
	$(AS) -c crt032.S -o bin/crt032.o --32 

bin/stdlib64.o: stdlib.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o bin/stdlib64.o

bin/stdlib32.o: stdlib.c 
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c stdlib.c -o bin/stdlib32.o

bin/exit32.o: calls/exit.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/exit.c -o bin/exit64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/exit.c -o bin/exit32.o

bin/getc32.o: calls/getc.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/getc.c -o bin/getc64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/getc.c -o bin/getc32.o

bin/strlen32.o: calls/strlen.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/strlen.c -o bin/strlen64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/strlen.c -o bin/strlen32.o

bin/printf32.o: calls/printf.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/printf.c -o bin/printf64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/printf.c -o bin/printf32.o

bin/puts32.o: calls/puts.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/puts.c -o bin/puts64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/puts.c -o bin/puts32.o

bin/putsc32.o: calls/putsc.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/putsc.c -o bin/putsc64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/putsc.c -o bin/putsc32.o

bin/convert32.o: calls/convert.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/convert.c -o bin/convert64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/convert.c -o bin/convert32.o

bin/__stack_chk_fail32.o: calls/__stack_chk_fail.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/__stack_chk_fail.c -o bin/__stack_chk_fail64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/__stack_chk_fail.c -o bin/__stack_chk_fail32.o

bin/hang32.o: calls/hang.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/hang.c -o bin/hang64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/hang.c -o bin/hang32.o

bin/send_tcp_message32.o: calls/send_tcp_message.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/send_tcp_message.c -o bin/send_tcp_message64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/send_tcp_message.c -o bin/send_tcp_message32.o

bin/start_tcp_session32.o: calls/start_tcp_session.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/start_tcp_session.c -o bin/start_tcp_session64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/start_tcp_session.c -o bin/start_tcp_session32.o

bin/wait_for_character32.o: calls/wait_for_character.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/wait_for_character.c -o bin/wait_for_character64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/wait_for_character.c -o bin/wait_for_character32.o

bin/scan_for_character32.o: calls/scan_for_character.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/scan_for_character.c -o bin/scan_for_character64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/scan_for_character.c -o bin/scan_for_character32.o

bin/scanf32.o: calls/scanf.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/scanf.c -o bin/scanf64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/scanf.c -o bin/scanf32.o

bin/memcpy32.o: calls/memcpy.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/memcpy.c -o bin/memcpy64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/memcpy.c -o bin/memcpy32.o

bin/draw_pixel32.o: calls/draw_pixel.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/draw_pixel.c -o bin/draw_pixel64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/draw_pixel.c -o bin/draw_pixel32.o

bin/fclose32.o: calls/fclose.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/fclose.c -o bin/fclose64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/fclose.c -o bin/fclose32.o

bin/fread32.o: calls/fread.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/fread.c -o bin/fread64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/fread.c -o bin/fread32.o

bin/fseek32.o: calls/fseek.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/fseek.c -o bin/fseek64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/fseek.c -o bin/fseek32.o

bin/ftell32.o: calls/ftell.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/ftell.c -o bin/ftell64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/ftell.c -o bin/ftell32.o

bin/fopen32.o: calls/fopen.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/fopen.c -o bin/fopen64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/fopen.c -o bin/fopen32.o

bin/memset32.o: calls/memset.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/memset.c -o bin/memset64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/memset.c -o bin/memset32.o

bin/malloc32.o: calls/malloc.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/malloc.c -o bin/malloc64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/malloc.c -o bin/malloc32.o

bin/free32.o: calls/free.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/free.c -o bin/free64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/free.c -o bin/free32.o

bin/scanline32.o: calls/scanline.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/scanline.c -o bin/scanline64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/scanline.c -o bin/scanline32.o

bin/strcpy32.o: calls/strcpy.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/strcpy.c -o bin/strcpy64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/strcpy.c -o bin/strcpy32.o

bin/time32.o: calls/time.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/time.c -o bin/time64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/time.c -o bin/time32.o

bin/write32.o: calls/write.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/write.c -o bin/write64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/write.c -o bin/write32.o

bin/fwrite32.o: calls/fwrite.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/fwrite.c -o bin/fwrite64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/fwrite.c -o bin/fwrite32.o

bin/getenv32.o: calls/getenv.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/getenv.c -o bin/getenv64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/getenv.c -o bin/getenv32.o

bin/gettimeofday32.o: calls/gettimeofday.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/gettimeofday.c -o bin/gettimeofday64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/gettimeofday.c -o bin/gettimeofday32.o

bin/assert32.o: calls/assert.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/assert.c -o bin/assert64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/assert.c -o bin/assert32.o

bin/strcmp32.o: calls/strcmp.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/strcmp.c -o bin/strcmp64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/strcmp.c -o bin/strcmp32.o

bin/calloc32.o: calls/calloc.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/calloc.c -o bin/calloc64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/calloc.c -o bin/calloc32.o

bin/strncpy32.o: calls/strncpy.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/strncpy.c -o bin/strncpy64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/strncpy.c -o bin/strncpy32.o

bin/ungetc32.o: calls/ungetc.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/ungetc.c -o bin/ungetc64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/ungetc.c -o bin/ungetc32.o

bin/realpath32.o: calls/realpath.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/realpath.c -o bin/realpath64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/realpath.c -o bin/realpath32.o

bin/putchar32.o: calls/putchar.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/putchar.c -o bin/putchar64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/putchar.c -o bin/putchar32.o

bin/sprintf32.o: calls/sprintf.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/sprintf.c -o bin/sprintf64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/sprintf.c -o bin/sprintf32.o

bin/fprintf32.o: calls/fprintf.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/fprintf.c -o bin/fprintf64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/fprintf.c -o bin/fprintf32.o

bin/vfprintf32.o: calls/vfprintf.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/vfprintf.c -o bin/vfprintf64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/vfprintf.c -o bin/vfprintf32.o

bin/vsnprintf32.o: calls/vsnprintf.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/vsnprintf.c -o bin/vsnprintf64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/vsnprintf.c -o bin/vsnprintf32.o

bin/realloc32.o: calls/realloc.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/realloc.c -o bin/realloc64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/realloc.c -o bin/realloc32.o

bin/strtol32.o: calls/strtol.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/strtol.c -o bin/strtol64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/strtol.c -o bin/strtol32.o

bin/vsprintf32.o: calls/vsprintf.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/vsprintf.c -o bin/vsprintf64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/vsprintf.c -o bin/vsprintf32.o

bin/fputc32.o: calls/fputc.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/fputc.c -o bin/fputc64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/fputc.c -o bin/fputc32.o

bin/tolower32.o: calls/tolower.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/tolower.c -o bin/tolower64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/tolower.c -o bin/tolower32.o

bin/isalpha32.o: calls/isalpha.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/isalpha.c -o bin/isalpha64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/isalpha.c -o bin/isalpha32.o

bin/atoll32.o: calls/atoll.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/atoll.c -o bin/atoll64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/atoll.c -o bin/atoll32.o

bin/system32.o: calls/system.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/system.c -o bin/system64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/system.c -o bin/system32.o

bin/cls32.o: calls/cls.c 
	$(CC) -nostdlib -m64 -I "./include" -ffreestanding -fno-PIC -c calls/cls.c -o bin/cls64.o
	$(CC) -nostdlib -m32 -I "./include" -ffreestanding -fno-PIC -c calls/cls.c -o bin/cls32.o