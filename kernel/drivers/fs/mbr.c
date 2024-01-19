#include "../../include/fs/fat.h"
#include "../../include/fs/ext.h"
#include "../../include/fs/mbr.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"

mbr_entry* getMBRRecordsFromBuffer(void *buffer){
    mbr_entry* mbre = (mbr_entry*) (buffer+0x1BE);
}

void initialise_fs(Blockdevice *dev,void* buffer){
    mbr_entry* fe = getMBRRecordsFromBuffer(buffer);
    char* efi = (char*) buffer;
    char found = 0;
    int foundcount = 0;
    for(int i = 0 ; i < 4 ; i++){
        if(fe[i].type==0xEE){
            k_printf("mbr: efi found!\n");
            foundcount++;
            dev->offset = fe[i].lbastart;
            k_printf("mbr: updating dev offset to %d \n",dev->offset);
            if(!device_read_raw_sector(dev,1,2,(void*)(buffer+512))){
                k_printf("device: unable to read sector!\n");
                return;
            }
            efi_entry* efis = (efi_entry*) (buffer+512);
            for(int z = 0 ; z < 8 ; z++){
                efi_entry efit = efis[z];
                if(efit.Partition_Type_GUID[14]==0x99&&efit.Partition_Type_GUID[15]==0xC7){
                    k_printf("mbr: we are in the right GUID\n");
                    dev->offset = efit.StartingLBA;
                    if(!device_read_raw_sector(dev,0,1,(void*)(buffer+512+512+512))){ // take a sample offset because it doesnt mean it is what we think it is...
                        k_printf("device: unable to read sector!\n");
                        return;
                    }
                    FATBootBlock* fbb = (FATBootBlock*) (buffer+512+512+512);
                    if((fbb->oem_name[0]=='F'&&fbb->oem_name[1]=='A'&&fbb->oem_name[2]=='T'&&fbb->oem_name[3]=='1'&&fbb->oem_name[4]=='6')||(fbb->oem_name[0]=='m'&&fbb->oem_name[1]=='k'&&fbb->oem_name[2]=='f'&&fbb->oem_name[3]=='s'&&fbb->oem_name[4]=='.')){
                        fat_detect_and_initialise(dev,buffer);
                        continue;
                    }
                }else{
                    k_printf("mbr: unknown GUID %x %x \n",efit.Partition_Type_GUID[14],efit.Partition_Type_GUID[15]);
                }
            }
            if(found){
                continue;
            }
        }else if(fe[i].type==0x06){
            dev->offset = fe[i].lbastart;
            foundcount++;
            k_printf("mbr: fat found!\n");
            fat_detect_and_initialise(dev,buffer);
        }else if(fe[i].type==0x83){
            dev->offset = fe[i].lbastart;
            foundcount++;
            k_printf("mbr: ext found!\n");
            ext_detect_and_initialise(dev,buffer);
        }else if(fe[i].type==0){
            // empty, simply ignore
            k_printf("mbr: entry %d is empty!\n",i);
        }else{
            foundcount++;
            k_printf("mbr: unknown type: %x \n",fe[i].type);for(;;);
        }
    }
    if(foundcount==0){
        dev->offset = 0;
        fat_detect_and_initialise(dev,buffer);
    }
}