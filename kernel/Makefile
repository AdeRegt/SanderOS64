all: build_asm_files build_c_files

build_asm_files:
	nasm asmstub64.asm -f elf64 -o asmstub64.o
	nasm asmstub32.asm -O0 -f elf32 -o asmstub32.o

CSOURCES := $(wildcard */*.c */*/*.c *.c)
build_c_files: $(CSOURCES)
	gcc -o ./../bin/sanderos/kernel64.bin $(CSOURCES) asmstub64.o -m64 -ffreestanding -mgeneral-regs-only -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Werror -ffreestanding -Xlinker -T ./linker64.ld -nostdlib -lgcc -ffreestanding
	gcc -o ./../bin/sanderos/kernel32.bin $(CSOURCES) asmstub32.o -m32 -ffreestanding -mgeneral-regs-only -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Werror -ffreestanding -Xlinker -T ./linker32.ld -nostdlib -ffreestanding
	rm *.o