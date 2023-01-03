all: bootsector.bin stage2.bin

bootsector.bin: bootsector.asm
	nasm -O0 -w+orphan-labels -f bin -o bootsector.bin bootsector.asm

stage2.bin: stage2.asm 
	nasm -O0 -w+orphan-labels -f bin -o stage2.bin stage2.asm