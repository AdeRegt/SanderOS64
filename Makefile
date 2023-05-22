all:
	$(MAKE) -C drivers
	$(MAKE) -C kernel
	$(MAKE) -C programsapi

create_programs:
	$(MAKE) -C programs

uefi_loader:
	$(MAKE) -C bootloader/UEFI ARCH=x86_64 BIT=64

grub_loader:
	$(MAKE) -C bootloader/GRUB32

update_image:

update_iso:
	grub-mkrescue -o cdrom.iso bin