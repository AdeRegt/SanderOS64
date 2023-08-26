#include "../../include/device.h"

typedef struct{
    uint32_t number_of_inodes_in_system;
    uint32_t total_number_of_block_in_filesystem;
    uint32_t number_of_blocks_reserved_for_superuser;
    uint32_t number_of_unallocated_blocks;
    uint32_t number_of_unallocated_inodes;
    uint32_t superblock_number;
    uint32_t log_blocksize;
    uint32_t log_fragmentsize;
    uint32_t blocks_in_blockgroup;
    uint32_t fragments_in_blockgroup;
    uint32_t inodes_per_blockgroup;
    uint32_t mounttime;
    uint32_t writetime;
    uint16_t opentimecount;
    uint16_t errormex;
    uint16_t checksum;
    uint16_t fsstate;
    uint16_t errorhandler;
    uint16_t minorversion;
    uint32_t posixconsistencycheck;
    uint32_t posixinterval;
    uint32_t creatoros;
    uint32_t majorversion;
    uint16_t userIDresevedblocks;
    uint16_t groupIDreservedblocks;

    // if  version older then 1
    uint32_t first_non_reserved_inode;
    uint16_t size_of_each_inode_structure_in_bytes;
    uint16_t blockgroup_of_supergroup;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t features_if_not_supported_readonly;
    uint8_t filesystemid[16];
    uint8_t volumename[16];
    uint8_t volumepath[64];
    uint32_t compression;
    uint8_t number_blocks_allocated_for_files;
    uint8_t number_blocks_allocated_for_dirs;
    uint16_t reserved;
    uint8_t journalid[16];
    uint32_t journalinode;
    uint32_t journaldevice;
    uint32_t headoforphan;
}__attribute__((packed)) EXTBaseSuperblockField;

void ext_detect_and_initialise(Blockdevice *dev,void* buffer);