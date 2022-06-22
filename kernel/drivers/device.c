#include "../include/device.h"

Blockdevice blockdevices[10];
int blockdevicemaxpointer = 0;

Blockdevice* getFreeBlockDevice(){
    Blockdevice* dev = (Blockdevice*) &blockdevices[blockdevicemaxpointer++];
    return dev;
}

Blockdevice* registerBlockDevice(uint64_t blocksize, void* readcommand, void* writecommand, uint64_t offset, void* attachment){
    Blockdevice* dev = getFreeBlockDevice();
    dev->blocksize = blocksize;
    dev->readcommand = readcommand;
    dev->writecommand = readcommand;
    dev->offset = offset;
    dev->attachment = attachment;
    return dev;
}

char device_read_raw_sector(Blockdevice* dev, uint64_t sector, uint32_t counter, void* buffer){
    unsigned char (*foo)(Blockdevice*, uint64_t, uint32_t, void*) = (void*)dev->readcommand;
	return foo(dev,dev->offset + sector,counter,buffer);
}