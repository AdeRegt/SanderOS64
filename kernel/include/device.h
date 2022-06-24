#pragma once
#include <stdint.h>

typedef struct{
    uint64_t blocksize;
    void* readcommand;
    void* writecommand;
    uint64_t offset;
    void* attachment;
}__attribute__((packed)) Blockdevice;

typedef struct{
    Blockdevice *blockdevice;
    void *readfile;
    void *dir;
    void *argument;
}__attribute__((packed)) Filesystem;

Blockdevice* registerBlockDevice(uint64_t blocksize, void* readcommand, void* writecommand, uint64_t offset, void* attachment);
Filesystem* registerFileSystem(Blockdevice *bd,void *read,void *dir);

char device_read_raw_sector(Blockdevice* dev, uint64_t sector, uint32_t counter, void* buffer);
char* dir(char* basepath);
char readFile(char* path,void *buffer);