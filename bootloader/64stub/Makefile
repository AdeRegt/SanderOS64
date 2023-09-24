all: build_asm_files build_c_files

build_asm_files:
	nasm -felf64 boot.asm -o boot.o || exit

CSOURCES := $(wildcard */*.c */*/*.c *.c)
build_c_files: $(CSOURCES)
	gcc -o ../../bin/sanderos/64stub.bin boot.o $(CSOURCES) -m64 -ffreestanding -mgeneral-regs-only -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Werror -ffreestanding -Xlinker -T linker.ld -nostdlib -ffreestanding