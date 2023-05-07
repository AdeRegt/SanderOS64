#include "../../include/fs/ext.h"
#include "../../include/fs/mbr.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"

void ext_detect_and_initialise(Blockdevice *dev,void* buffer){
    k_printf("ext: driver initialised!\n");
    EXTBaseSuperblockField *bpb = (EXTBaseSuperblockField*) requestPage();
    if(!device_read_raw_sector(dev,2,1,(void*)bpb)){
        k_printf("device: unable to read sector for the fat info!\n");
        return;
    }
    k_printf("ext: number_of_inodes_in_system: %d total_number_of_block_in_filesystem: %d \n",bpb->number_of_inodes_in_system,bpb->total_number_of_block_in_filesystem);
    k_printf("ext: number_of_unallocated_blocks: %d number_of_unallocated_inodes: %d \n",bpb->number_of_unallocated_blocks,bpb->number_of_unallocated_inodes);
    k_printf("ext: superblock_number: %d checksum: %x min:%x maj:%x \n",bpb->superblock_number,bpb->checksum,bpb->minorversion,bpb->majorversion);
    if(bpb->checksum!=0xef53){
        k_printf("ext: invalid checksum\n");
        return;
    }
    if(bpb->majorversion<1){
        k_printf("ext: invalid version\n");
        return;
    }
    k_printf("ext: required features flags: %x blocksize: %x \n",bpb->required_features,1024 << bpb->log_blocksize);
}