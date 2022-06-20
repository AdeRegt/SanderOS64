#include "../../include/fs/mbr.h"

mbr_entry* getMBRRecordsFromBuffer(void *buffer){
    mbr_entry* mbre = (mbr_entry*) (buffer+0x1BE);
}