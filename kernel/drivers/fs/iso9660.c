#include "../../include/fs/iso9660.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"

upointer_t iso9660_filesize(Filesystem* fs,char* path){
    return 0;
}

char iso9660_read(Filesystem* fs,char* path,void *bufferedcapacity){
    return 0;
}

char *iso9660_dir(Filesystem* fs,char* path){
    ISO9660PrimaryVolumeDescriptor* bpb = (ISO9660PrimaryVolumeDescriptor*) fs->argument;

    // load directory tables
    void *directorybuffer = requestPage();
    if(!device_read_raw_sector(fs->blockdevice,0x10,1,(void*)directorybuffer)){
        k_printf("iso9660: unable to read sector for the fat info!\n");
        return 0;
    }
    uint32_t directory_lba = ((uint32_t*)(directorybuffer + 2))[0];
    k_printf("iii: %d \n",directory_lba);

    // load directory
    char* result = (char*) requestPage();
    memset(result,0,100);
    return result;
}

upointer_t iso9660_write(Filesystem* fs,char* path,void* bufferedcapacity,upointer_t filesize){
    return 0;
}

void initialise_iso9660(Blockdevice* dev){
    k_printf("iso9660: initialising iso9660 driver!\n");

    ISO9660PrimaryVolumeDescriptor *bpb = (ISO9660PrimaryVolumeDescriptor*) requestPage();
    memset(bpb,0,sizeof(ISO9660PrimaryVolumeDescriptor));
    if(!device_read_raw_sector(dev,0x10,1,(void*)bpb)){
        k_printf("iso9660: unable to read sector for the fat info!\n");
        return;
    }

    // check headers...
    if(bpb->signature.type!=ISO9660_VOLUME_DESCRIPTORS_PRIMARY_VOLUME_DESCRIPTOR){
        k_printf("iso9660: Invalid field descriptor type\n");
        return;
    }

    if(bpb->signature.version!=ISO9660_PROTOCOL_VERSION){
        k_printf("iso9660: Invalid field descriptor version\n");
        return;
    }

    if(
        !(
            bpb->signature.identifier[0]=='C'&&
            bpb->signature.identifier[1]=='D'&&
            bpb->signature.identifier[2]=='0'&&
            bpb->signature.identifier[3]=='0'&&
            bpb->signature.identifier[4]=='1'
        )
    ){
        k_printf("iso9660: Invalid field descriptor signature\n");
        return;
    }

    if((bpb->logical_block_size & 0xFFFF)!=ISO9660_LOGICAL_BLOCK_SIZE){
        k_printf("iso9660: Invalid logical block size\n");
        return;
    }

    ISO9660Argument* arg = (ISO9660Argument*) requestPage();
    arg->descriptor = bpb;
    arg->directorytables = requestPage();

    if(!device_read_raw_sector((Blockdevice*)dev,switch_endian32(bpb->location_of_the_M_path_table),1,(void*)arg->directorytables)){
        k_printf("iso9660: unable to read sector for the fat directory table!\n");
        return;
    }

    Filesystem *fs = registerFileSystem(dev,iso9660_read,iso9660_dir,iso9660_filesize,iso9660_write);
    fs->argument = arg;
}