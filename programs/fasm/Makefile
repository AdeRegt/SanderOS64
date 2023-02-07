all:
	mkdir origin/SOURCE/SANDEROS64
	mkdir origin/SOURCE/SANDEROS64/X64
	cp ./fasm64.asm origin/SOURCE/SANDEROS64/X64/fasm.asm
	cp ./linker64.ld origin/SOURCE/SANDEROS64/X64/linker.ld
	cp ./modes64.inc origin/SOURCE/SANDEROS64/X64/modes.inc
	cp ./system64.inc origin/SOURCE/SANDEROS64/X64/system.inc
	cp ./Copymake origin/SOURCE/SANDEROS64/X64/Makefile
	$(MAKE) -C origin/SOURCE/SANDEROS64/X64
	rm origin/SOURCE/SANDEROS64/X64/Makefile
	rm -r origin/SOURCE/SANDEROS64/X64
	rm -r origin/SOURCE/SANDEROS64
	cp ./fasm64.bin ../../bin/programs/64bit/fasm.bin