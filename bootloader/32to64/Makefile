all: build_asm_files build_c_files

build_asm_files:
	nasm -felf32 boot.asm -o boot.o || exit

CSOURCES := $(wildcard */*.c */*/*.c *.c)
build_c_files: $(CSOURCES)
	gcc -o ../../bin/sanderos/32to64.bin boot.o $(CSOURCES) -m32 -ffreestanding -mgeneral-regs-only -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Werror -ffreestanding -Xlinker -T linker.ld -nostdlib -ffreestanding