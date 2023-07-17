all:
	mkdir origin/SOURCE/SANDEROS64
	mkdir origin/SOURCE/SANDEROS64/X64
	cp ./fasm64.asm origin/SOURCE/SANDEROS64/X64/fasm.asm
	cp ./fasm32.asm origin/SOURCE/SANDEROS64/fasm.asm
	cp ./linker64.ld origin/SOURCE/SANDEROS64/X64/linker.ld
	cp ./linker32.ld origin/SOURCE/SANDEROS64/linker.ld
	cp ./modes64.inc origin/SOURCE/SANDEROS64/X64/modes.inc
	cp ./system64.inc origin/SOURCE/SANDEROS64/X64/system.inc
	cp ./system32.inc origin/SOURCE/SANDEROS64/system.inc
	cp ./Copymake64 origin/SOURCE/SANDEROS64/X64/Makefile
	cp ./Copymake32 origin/SOURCE/SANDEROS64/Makefile
	$(MAKE) -C origin/SOURCE/SANDEROS64/X64
	rm origin/SOURCE/SANDEROS64/X64/Makefile
	rm -r origin/SOURCE/SANDEROS64/X64
	$(MAKE) -C origin/SOURCE/SANDEROS64
	rm origin/SOURCE/SANDEROS64/Makefile
	rm -r origin/SOURCE/SANDEROS64
	cp ./fasm64.bin ../../bin/programs/64bit/fasm.bin
	cp ./fasm32.bin ../../bin/programs/32bit/fasm.bin