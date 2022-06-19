FS0:
if exist \sanderos\kernel.bin then 
 \sanderos\SANDEROS64.EFI 
 goto END
endif

 echo "Unable to find bootloader".
 
:END