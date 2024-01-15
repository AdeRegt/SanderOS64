qemu-img create -f raw hdd.img 50M
sed -e 's/\s*\([\+0-9a-zA-Z]*\).*/\1/' << EOF | fdisk hdd.img
  o
  n
  p
  1

  
  t
  1
  w
  q
EOF
mkfs.vfat hdd.img
mkdir test
sudo mount hdd.img ./test
sudo cp -R ./bin/* ./test
sudo umount ./test
rm -rf test
qemu-system-x86_64 -drive id=disk,file=hdd.img,if=none -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="../OVMF.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="../OVMF.fd" -net none