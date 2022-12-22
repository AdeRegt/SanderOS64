all: build_asm_files build_c_files

build_asm_files:
	nasm asmstub.asm -f elf64 -o asmstub.o

CSOURCES := $(wildcard */*.c */*/*.c *.c)
build_c_files: $(CSOURCES)
	gcc -o ./../bin/sanderos/kernel64.bin $(CSOURCES) asmstub.o -m64 -ffreestanding -mgeneral-regs-only -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Werror -ffreestanding -Xlinker -T ./linker64.ld -nostdlib -lgcc -ffreestanding
