#include "../../include/fs/fat.h"
#include "../../include/fs/mbr.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"

upointer_t fat_filesize(Filesystem* fs,char* path){
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
        if(path[i]>='a'&&path[i]<='z'){
            path[i] = path[i] + ('A'-'a');
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

    return filesize;
}

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
        if(path[i]>='a'&&path[i]<='z'){
            path[i] = path[i] + ('A'-'a');
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

    uint32_t toreadsectors = 0;//(filesize/fs->blockdevice->blocksize)+1;
    uint32_t ssc = 0;
    while(1){
        ssc += fs->blockdevice->blocksize;
        toreadsectors++;
        if(ssc>= filesize){
            break;
        }
    }
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
        if(path[i]>='a'&&path[i]<='z'){
            path[i] = path[i] + ('A'-'a');
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
            if(u[i].filename[0]==0xE5){ // it means the file is unused
                continue;
            }
            
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

upointer_t fat_write(Filesystem* fs,char* path,void* bufferedcapacity,upointer_t filesize){
    FATFileSystemSettings* settings = (FATFileSystemSettings*) fs->argument;
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
        if(path[i]>='a'&&path[i]<='z'){
            path[i] = path[i] + ('A'-'a');
        }
        if(path[i]=='/'){
            count_of_slashes++;
        }
        i++;
        stringcount++;
    }
    uint32_t secset = settings->first_root_dir_sector;
    uint32_t filesizeold = 0;
    void *es = (void*) requestPage();
    FATFileDefinition* u = (FATFileDefinition*) es;
    i = 0;
    char buffer[17];
    int found_solution_for_this_part = 0;
    int index_of_new_or_changed_file = 0;
    uint32_t lastdirsect = 0;
    for(int segment_id = 0 ; segment_id < count_of_slashes ; segment_id++){
        memset(&buffer,0,17);
        int z = 0;
        found_solution_for_this_part = 0;
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
        lastdirsect = secset;
        if(!device_read_raw_sector(fs->blockdevice,secset,1,(void*)es)){
            freePage(es);
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
                    rawcalc *= settings->bb->sectors_per_cluster;
                    rawcalc += settings->first_data_sector;

                    secset = rawcalc;
                    filesizeold = u[q].size;

                    index_of_new_or_changed_file = q;

                    found_solution_for_this_part = 1;
                }
            }
        }
        if( found_solution_for_this_part==0 && ( segment_id != (count_of_slashes-1) ) ){
            return 0;
        }
    }

    if(found_solution_for_this_part==0){
        for(int q = 0 ; q < 16 ; q++){
            if(u[q].attributes==0){
                u[q].attributes = 0x20;
                u[q].cluster_high = 0;
                u[q].size = filesize;
                int w = 0;
                // fill with space
                for(int z = 0 ; z < 11 ; z++){
                    u[q].filename[z] = ' ';
                }
                // put in the filename
                for(int z = 0 ; z < 11 ; z++){
                    if(buffer[w]=='.'){
                        w++;
                        z = 7;
                        continue;
                    }
                    u[q].filename[z] = buffer[w];
                    w++;
                }
                char* z = (char*) &u[q];
                found_solution_for_this_part = 1;
                index_of_new_or_changed_file = q;
                break;
            }
        }
    }

    if(filesize > (settings->bb->sectors_per_cluster*settings->bb->bytes_per_sector)){
        freePage(es);
        k_printf("Unsupported filesize!!!\n");
        for(;;);
        return 0;
    }

    u[index_of_new_or_changed_file].size = filesize;

    if(u[index_of_new_or_changed_file].cluster_low==0){
        upointer_t *chdv = (upointer_t*)requestPage();
        if(!device_read_raw_sector(fs->blockdevice,settings->first_fat_sector,1,(void*)chdv)){
            freePage(chdv);
            freePage(es);
            k_printf("device: unable to read sector!\n");
            return 0;
        }

        for(uint16_t i = 100 ; i < (settings->bb->bytes_per_sector/sizeof(uint16_t)) ; i++){
            uint16_t vl = (uint16_t) chdv[i] & 0xFFFF;
            if(vl==0){
                chdv[i] = 0xFFF8;
                u[index_of_new_or_changed_file].cluster_low = i;
                break;
            }
        }

        if(!device_write_raw_sector(fs->blockdevice,settings->first_fat_sector,1,(void*)chdv)){
            freePage(chdv);
            freePage(es);
            k_printf("device: unable to write sector for the fat root dir!\n");
            return 0;
        }
        freePage(chdv);
    }

    if(!device_write_raw_sector(fs->blockdevice,lastdirsect,1,(void*)u)){
        freePage(es);
        k_printf("device: unable to write sector for the fat root dir!\n");
        return 0;
    }

    uint16_t bigaddr[2];
    bigaddr[0] = u[index_of_new_or_changed_file].cluster_low;
    bigaddr[1] = u[index_of_new_or_changed_file].cluster_high;
    uint32_t rawcalc = ((uint32_t*)&bigaddr)[0];
    rawcalc -= 2;
    rawcalc *= settings->bb->sectors_per_cluster;
    rawcalc += settings->first_data_sector;

    if(!device_write_raw_sector(fs->blockdevice,rawcalc ,(filesize/512)+1,(void*)bufferedcapacity)){
        freePage(es);
        k_printf("device: unable to write sector for the fat root dir!\n");
        return 0;
    }
    freePage(es);

    return filesize;
}

void fat_detect_and_initialise(Blockdevice *dev,void* buffer){
    
    FATBootBlock *bpb = (FATBootBlock*) requestPage();
    memset(bpb,0,sizeof(FATBootBlock));
    if(!device_read_raw_sector(dev,0,1,(void*)bpb)){
        k_printf("device: unable to read sector for the fat info!\n");
        return;
    }
    k_printf("fat: bytes per sector: %d , oemname: %s , root entry count: %d , lbastart: %d \n",bpb->bytes_per_sector,bpb->oem_name,bpb->root_entry_count,dev->offset);
    uint32_t total_sectors = (bpb->total_sectors_16 == 0)? bpb->total_sectors_32 : bpb->total_sectors_16;
    uint32_t fat_size = (bpb->table_size_16 == 0)? bpb->table_size_16 : bpb->table_size_16;
    uint32_t root_dir_sectors = ((bpb->root_entry_count * 32) + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector;
    uint32_t first_data_sector = bpb->reserved_sector_count + (bpb->table_count * fat_size) + root_dir_sectors;
    uint32_t first_fat_sector = bpb->reserved_sector_count;
    uint32_t first_root_dir_sector = first_data_sector - root_dir_sectors;

    Filesystem *fs = registerFileSystem(dev,fat_read,fat_dir,fat_filesize,fat_write);
    FATFileSystemSettings *fss = (FATFileSystemSettings*) requestPage();
    fss->bb = bpb;
    fss->fat_size = fat_size;
    fss->first_data_sector = first_data_sector;
    fss->first_fat_sector = first_fat_sector;
    fss->first_root_dir_sector = first_root_dir_sector;
    fss->root_dir_sectors = root_dir_sectors;
    fss->total_sectors = total_sectors;

    fs->argument = fss;
    k_printf("fat: end of initialisation\n");
}