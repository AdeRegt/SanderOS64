all:
	$(MAKE) -C drivers
	$(MAKE) -C kernel
	$(MAKE) -C programsapi

create_programs:
	$(MAKE) -C programs

uefi_loader:
	$(MAKE) -C bootloader/UEFI ARCH=x86_64 BIT=64

grub_loader:
	$(MAKE) -C bootloader/64stub
	$(MAKE) -C bootloader/32to64

update_image:

update_iso:
	grub-mkrescue -o cdrom.iso bin

update_vhd:
	dd if=/dev/zero of=./VHD.img bs=1M count=1200
	mkfs -t fat ./VHD.img
	mkdir ./loop 
	mount -t auto -o loop ./VHD.img ./loop
	cp -R bin/* ./loop
	umount ./loop 
	sleep 5
	rm -rf ./loop 
	qemu-img convert -f raw -O vmdk ./VHD.img ./vhdnew.vmdk
	chmod 777 ./vhdnew.vmdk