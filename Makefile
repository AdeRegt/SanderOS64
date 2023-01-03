all:
	$(MAKE) -C bootloader/UEFI ARCH=x86_64 BIT=64
	$(MAKE) -C bootloader/GRUB32
	$(MAKE) -C drivers
	$(MAKE) -C kernel
	$(MAKE) -C programsapi
	$(MAKE) -C programs

update_image:
	mkdir vh
	guestmount -a ./hdd.vhd -m /dev/sda1 ./vh
	guestunmount ./vh
	sleep 5
	rm ./vh

update_iso:
	grub-mkrescue -o cdrom.iso bin