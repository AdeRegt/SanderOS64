all:
	mkdir origin/SOURCE/SANDEROS64
	mkdir origin/SOURCE/SANDEROS64/X64
	cp ./fasm.asm origin/SOURCE/SANDEROS64/X64/fasm.asm
	cp ./linker.ld origin/SOURCE/SANDEROS64/X64/linker.ld
	cp ./modes.inc origin/SOURCE/SANDEROS64/X64/modes.inc
	cp ./system.inc origin/SOURCE/SANDEROS64/X64/system.inc
	cd origin/SOURCE/SANDEROS64/X64
	fasm ./fasm.asm
	$(CC) -T ./linker.ld fasm.o -o ./fasmexec
	cp ./fasmexec ../../../../fasm.bin
	rm ./fasm.asm 
	rm ./fasm.o
	rm ./fasmexec 
	rm ./linker.ld 
	rm ./modes.inc 
	rm ./system.inc 
	cd ..
	rm -r X64
	cd .. 
	rm -r SANDEROS64
	cd ../..
	cp ./fasm.bin ../../bin/programs/fasm.bin