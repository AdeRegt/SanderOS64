all:
	cp origin/Makefile ./Makefilebackup
	cp ./Makefilechanged origin/Makefile
	$(MAKE) -C origin
	cp ./Makefilebackup origin/Makefile
	rm ./Makefilebackup