#include "../../include/fs/fat.h"
#include "../../include/fs/mbr.h"
#include "../../include/graphics.h"

void fat_detect_and_initialise(Blockdevice *dev,void* buffer){
    mbr_entry* fe = getMBRRecordsFromBuffer(buffer);
    char* efi = (char*) buffer;
    for(int i = 0 ; i < 4 ; i++){
        if(fe[i].type==0xEE){
            k_printf("mbr: efi found!\n");
            if(!device_read_raw_sector(dev,fe[i].lbastart+1,1,(void*)(buffer+512))){
                k_printf("device: unable to read sector!\n");
            }
            for(int z = 512 ; z < 1024 ; z = z+128){
                if(((efi[z+8+6]&0xFF)==0xC9&&(efi[z+8+7]&0xFF)==0x3B)||((efi[z+8+6]&0xFF)==0x99&&(efi[z+8+7]&0xFF)==0xC7)){
                    k_printf("efi_header: found FAT partition!\n");
                }else{
                    k_printf("efi_header: unknown header %x %x \n",(efi[z+8+6]&0xFF),(efi[z+8+7]&0xFF));
                }
            }
        }
    }
    for(;;);
}