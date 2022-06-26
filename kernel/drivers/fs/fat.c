#include "../../include/fs/fat.h"
#include "../../include/fs/mbr.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"

char fat_read(Filesystem* fs,char* path,void *bufferedcapacity){
    FATFileSystemSettings *fss = (FATFileSystemSettings*) fs->argument;
    int count_of_slashes = 0;
    int stringcount = 0;
    if(path[0]!=0){
        count_of_slashes++;
    }
    int i = 0;
    while(1){
        if(path[i]==0){
            break;
        }
        if(path[i]=='/'){
            count_of_slashes++;
        }
        i++;
        stringcount++;
    }
    uint32_t secset = fss->first_root_dir_sector;
    uint32_t filesize = 0;
    void *es = (void*) requestPage();
    FATFileDefinition* u = (FATFileDefinition*) es;
    i = 0;
    for(int segment_id = 0 ; segment_id < count_of_slashes ; segment_id++){
        char buffer[16];
        memset(&buffer,0,16);
        int z = 0;
        int found_solution_for_this_part = 0;
        while(1){
            char deze = path[i];
            if(deze==0||deze=='/'){
                i++;
                break;
            }
            buffer[z] = deze;
            z++;
            i++;
            if(i==stringcount){
                break;
            }
        }
        memset(es,0,512);
        if(!device_read_raw_sector(fs->blockdevice,secset,1,(void*)es)){
            k_printf("device: unable to read sector for the fat root dir!\n");
            return 0;
        }
        for(int q = 0 ; q < 16 ; q++){
            if(u[q].attributes==0x20||u[q].attributes==0x10){
                int gz = 1;
                int w = 0;
                for(int z = 0 ; z < 11 ; z++){
                    if( buffer[w] == '.' ){
                        w++;
                    }
                    if(u[q].filename[z]==' '||u[q].filename[z]==0x00){
                        continue;
                    }
                    if( buffer[w] != u[q].filename[z] ){
                        gz = 0;
                    }
                    w++;
                }
                if(gz){
                    uint16_t bigaddr[2];
                    bigaddr[0] = u[q].cluster_low;
                    bigaddr[1] = u[q].cluster_high;
                    uint32_t rawcalc = ((uint32_t*)&bigaddr)[0];
                    rawcalc -= 2;
                    rawcalc *= fss->bb->sectors_per_cluster;
                    rawcalc += fss->first_data_sector;

                    secset = rawcalc;
                    filesize = u[q].size;

                    found_solution_for_this_part = 1;
                }
            }
        }
        if(found_solution_for_this_part==0){
            return 0;
        }
    }

    uint32_t toreadsectors = (filesize/fs->blockdevice->blocksize)+1;
    return device_read_raw_sector(fs->blockdevice,secset,toreadsectors,bufferedcapacity);
}

char *fat_dir(Filesystem* fs,char* path){
    FATFileSystemSettings *fss = (FATFileSystemSettings*) fs->argument;
    char *resultstring = (char*) requestPage();
    memset(resultstring,0,512);
    int resultstringpointer = 0;
    int count_of_slashes = 0;
    int stringcount = 0;
    if(path[0]!=0){
        count_of_slashes++;
    }
    int i = 0;
    while(1){
        if(path[i]==0){
            break;
        }
        if(path[i]=='/'){
            count_of_slashes++;
        }
        i++;
        stringcount++;
    }
    uint32_t secset = fss->first_root_dir_sector;
    void *es = (void*) requestPage();
    FATFileDefinition* u = (FATFileDefinition*) es;
    i = 0;
    for(int segment_id = 0 ; segment_id < count_of_slashes ; segment_id++){
        char buffer[16];
        memset(&buffer,0,16);
        int z = 0;
        int found_solution_for_this_part = 0;
        while(1){
            char deze = path[i];
            if(deze==0||deze=='/'){
                i++;
                break;
            }
            buffer[z] = deze;
            z++;
            i++;
            if(i==stringcount){
                break;
            }
        }
        memset(es,0,512);
        if(!device_read_raw_sector(fs->blockdevice,secset,1,(void*)es)){
            k_printf("device: unable to read sector for the fat root dir!\n");
            return 0;
        }
        for(int q = 0 ; q < 16 ; q++){
            if(u[q].attributes==0x20||u[q].attributes==0x10){
                int gz = 1;
                for(int z = 0 ; z < 11 ; z++){
                    if(buffer[z] && u[i].filename[z]!=' ' ){
                        if( buffer[z] != u[q].filename[z] ){
                            gz = 0;
                        }
                    }
                }
                if(gz){
                    uint16_t bigaddr[2];
                    bigaddr[0] = u[q].cluster_low;
                    bigaddr[1] = u[q].cluster_high;
                    uint32_t rawcalc = ((uint32_t*)&bigaddr)[0];
                    rawcalc -= 2;
                    rawcalc *= fss->bb->sectors_per_cluster;
                    rawcalc += fss->first_data_sector;
                    secset = rawcalc;
                    found_solution_for_this_part = 1;
                }
            }
        }
        if(found_solution_for_this_part==0){
            return 0;
        }
    }

    memset(es,0,512);
    if(!device_read_raw_sector(fs->blockdevice,secset,1,(void*)es)){
        k_printf("device: unable to read sector for the fat root dir!\n");
        return 0;
    }

    int g = 0;
    for(int i = 0 ; i < 16 ; i++){
        if(u[i].attributes==0x20||u[i].attributes==0x10){
            if(g){
                resultstring[resultstringpointer++] = ';';
            }
            for(int z = 0 ; z < 11 ; z++){
                if(z==8&&u[i].attributes==0x20){
                    resultstring[resultstringpointer++] = '.';
                }
                if( u[i].filename[z] && u[i].filename[z]!=' ' ){
                    resultstring[resultstringpointer++] = u[i].filename[z];
                }
            }
            g++;
        }
    }
    return resultstring;
}

void fat_detect_and_initialise(Blockdevice *dev,void* buffer){
    mbr_entry* fe = getMBRRecordsFromBuffer(buffer);
    char* efi = (char*) buffer;
    char found = 0;
    for(int i = 0 ; i < 4 ; i++){
        if(fe[i].type==0xEE){
            k_printf("mbr: efi found!\n");
            dev->offset = fe[i].lbastart;
            if(!device_read_raw_sector(dev,1,2,(void*)(buffer+512))){
                k_printf("device: unable to read sector!\n");
                return;
            }
            efi_entry* efis = (efi_entry*) (buffer+512);
            for(int z = 0 ; z < 8 ; z++){
                efi_entry efit = efis[z];
                if(efit.Partition_Type_GUID[14]==0x99&&efit.Partition_Type_GUID[15]==0xC7){
                    dev->offset = efit.StartingLBA;
                    if(!device_read_raw_sector(dev,0,1,(void*)(buffer+512+512+512))){ // take a sample offset because it doesnt mean it is what we think it is...
                        k_printf("device: unable to read sector!\n");
                        return;
                    }
                    FATBootBlock* fbb = (FATBootBlock*) (buffer+512+512+512);
                    if((fbb->oem_name[0]=='F'&&fbb->oem_name[1]=='A'&&fbb->oem_name[2]=='T'&&fbb->oem_name[3]=='1'&&fbb->oem_name[4]=='6')||(fbb->oem_name[0]=='m'&&fbb->oem_name[1]=='k'&&fbb->oem_name[2]=='f'&&fbb->oem_name[3]=='s'&&fbb->oem_name[4]=='.')){
                        found = 1;
                        continue;
                    }
                }
            }
            if(found){
                continue;
            }
        }else if(fe[i].type==0x06){
            dev->offset = fe[i].lbastart;
            found = 1;
        }
    }
    if(found){
        FATBootBlock *bpb = (FATBootBlock*) requestPage();
        memset(bpb,0,sizeof(FATBootBlock));
        if(!device_read_raw_sector(dev,0,1,(void*)bpb)){
            k_printf("device: unable to read sector for the fat info!\n");
            return;
        }
        uint32_t total_sectors = (bpb->total_sectors_16 == 0)? bpb->total_sectors_32 : bpb->total_sectors_16;
        uint32_t fat_size = (bpb->table_size_16 == 0)? bpb->table_size_16 : bpb->table_size_16;
        uint32_t root_dir_sectors = ((bpb->root_entry_count * 32) + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector;
        uint32_t first_data_sector = bpb->reserved_sector_count + (bpb->table_count * fat_size) + root_dir_sectors;
        uint32_t first_fat_sector = bpb->reserved_sector_count;
        uint32_t first_root_dir_sector = first_data_sector - root_dir_sectors;

        Filesystem *fs = registerFileSystem(dev,fat_read,fat_dir);
        FATFileSystemSettings *fss = (FATFileSystemSettings*) requestPage();
        fss->bb = bpb;
        fss->fat_size = fat_size;
        fss->first_data_sector = first_data_sector;
        fss->first_fat_sector = first_fat_sector;
        fss->first_root_dir_sector = first_root_dir_sector;
        fss->root_dir_sectors = root_dir_sectors;
        fss->total_sectors = total_sectors;

        fs->argument = fss;
    }
    k_printf("fat: end of initialisation\n");
}