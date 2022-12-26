all:
	$(MAKE) -C bootloader/UEFI
	$(MAKE) -C bootloader/GRUB32
	$(MAKE) -C drivers
	$(MAKE) -C kernel
	$(MAKE) -C programsapi
	$(MAKE) -C programs