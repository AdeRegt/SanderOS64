#pragma once
#include <stdint.h>

typedef struct{
    uint64_t blocksize;
    void* readcommand;
    void* writecommand;
    uint64_t offset;
    void* attachment;
}__attribute__((packed)) Blockdevice;

Blockdevice* registerBlockDevice(uint64_t blocksize, void* readcommand, void* writecommand, uint64_t offset, void* attachment);
char device_read_raw_sector(Blockdevice* dev, uint64_t sector, uint32_t counter, void* buffer);