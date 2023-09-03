all:
	cp origin/Makefile ./Makefilebackup
	cp ./Makefilechanged origin/Makefile
	$(MAKE) -C origin CC=~/opt/cross/bin/i386-elf-gcc
	cp ./Makefilebackup origin/Makefile
	rm ./Makefilebackup