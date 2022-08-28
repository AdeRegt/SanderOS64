echo "create bootstrap"
nasm -felf32 boot.asm -o boot.o
gcc -m32 -c loader.c -o loader.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -T linker.ld -o ../../bin/sanderos/grubloader.bin -ffreestanding -O2 -nostdlib boot.o loader.o
rm *.o
cp grub.cfg ../../bin/grub/grub.cfg 
