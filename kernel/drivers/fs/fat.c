#include "../../include/fs/fat.h"
#include "../../include/fs/mbr.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"

void fat_detect_and_initialise(Blockdevice *dev,void* buffer){
    mbr_entry* fe = getMBRRecordsFromBuffer(buffer);
    char* efi = (char*) buffer;
    char found = 0;
    for(int i = 0 ; i < 4 ; i++){
        if(fe[i].type==0xEE){
            k_printf("mbr: efi found!\n");
            dev->offset = fe[i].lbastart;
            if(!device_read_raw_sector(dev,1,1,(void*)(buffer+512))){
                k_printf("device: unable to read sector!\n");
                return;
            }
            if(!device_read_raw_sector(dev,2,1,(void*)(buffer+512+512))){
                k_printf("device: unable to read sector!\n");
                return;
            }
            efi_entry* efis = (efi_entry*) (buffer+512);
            for(int z = 0 ; z < 8 ; z++){
                efi_entry efit = efis[z];
                if(efit.Partition_Type_GUID[14]==0x99&&efit.Partition_Type_GUID[15]==0xC7){
                    dev->offset = efit.StartingLBA;
                    k_printf("efi_header: startingLBA: %d \n",efit.StartingLBA);
                    if(!device_read_raw_sector(dev,0,1,(void*)(buffer+512+512+512))){ // take a sample offset because it doesnt mean it is what we think it is...
                        k_printf("device: unable to read sector!\n");
                        return;
                    }
                    FATBootBlock* fbb = (FATBootBlock*) (buffer+512+512+512);
                    if((fbb->oem_name[0]=='F'&&fbb->oem_name[1]=='A'&&fbb->oem_name[2]=='T'&&fbb->oem_name[3]=='1'&&fbb->oem_name[4]=='6')||(fbb->oem_name[0]=='m'&&fbb->oem_name[1]=='k'&&fbb->oem_name[2]=='f'&&fbb->oem_name[3]=='s'&&fbb->oem_name[4]=='.')){
                        found = 1;
                        // dev->offset = sectoroffset;
                        k_printf("efi_header: found FAT partition at %d !\n",dev->offset);
                        continue;
                    }else{
                        k_printf("efi_header: invalid OEName %c%c%c%c%c!\n",fbb->oem_name[0],fbb->oem_name[1],fbb->oem_name[2],fbb->oem_name[3],fbb->oem_name[4]);
                    }
                }else{
                    k_printf("efi_header: guid %x %x \n",efit.Partition_Type_GUID[14],efit.Partition_Type_GUID[15]);
                }
            }
            if(found){
                continue;
            }
        }else if(fe[i].type==0x06){
            k_printf("mbr: found FAT partition!\n");
            dev->offset = fe[i].lbastart;
            found = 1;
        }
    }
    if(found){
        k_printf("fat: there is a fat partition found on this mbr!\n");
        FATBootBlock *bpb = (FATBootBlock*) requestPage();
        memset(bpb,0,sizeof(FATBootBlock));
        if(!device_read_raw_sector(dev,0,1,(void*)bpb)){
            k_printf("device: unable to read sector for the fat info!\n");
            return;
        }
        k_printf("0xEB:%x 0x3C:%x 0x90:%x \n",bpb->bootjmp[0],bpb->bootjmp[1],bpb->bootjmp[2]);
        for(int i = 0 ; i < 8 ; i++){
            k_printf("%c", bpb->oem_name[i]);
        }
        uint32_t total_sectors = (bpb->total_sectors_16 == 0)? bpb->total_sectors_32 : bpb->total_sectors_16;
        uint32_t fat_size = (bpb->table_size_16 == 0)? bpb->table_size_16 : bpb->table_size_16;
        uint32_t root_dir_sectors = ((bpb->root_entry_count * 32) + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector;
        uint32_t first_data_sector = bpb->reserved_sector_count + (bpb->table_count * fat_size) + root_dir_sectors;
        uint32_t first_fat_sector = bpb->reserved_sector_count;
        uint32_t first_root_dir_sector = first_data_sector - root_dir_sectors;
        k_printf("first root directory: %d \n",first_root_dir_sector);

        void *es = (void*) requestPage();
        memset(es,0,512);
        if(!device_read_raw_sector(dev,first_root_dir_sector,1,(void*)es)){
            k_printf("device: unable to read sector for the fat root dir!\n");
            return;
        }

        FATFileDefinition* u = (FATFileDefinition*) es;
        for(int i = 0 ; i < 16 ; i++){
            if(u[i].attributes==0x20||u[i].attributes==0x10){
                for(int z = 0 ; z < 11 ; z++){
                    k_printf("%c",u[i].filename[z]);
                }
                k_printf(", ");
            }
        }
        k_printf("\n");
    }
    k_printf("fat: end of initialisation\n");
    for(;;);
}