all: kernel drivers programsapi programs
	$(MAKE) -C bootloader/UEFI
	$(MAKE) -C bootloader/GRUB32

drivers: 

kernel:

programs: 

programsapi: