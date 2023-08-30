#pragma once
#include <stdint.h>

#define FILENAME_MAX 200

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
    void *filesize;
    void *write;
}__attribute__((packed)) Filesystem;

Blockdevice* registerBlockDevice(uint64_t blocksize, void* readcommand, void* writecommand, uint64_t offset, void* attachment);
Filesystem* registerFileSystem(Blockdevice *bd,void *read,void *dir,void *filesize,void *write);

void registerHIDDevice(void *getcpointer);
uint8_t *scanLine(uint64_t maxsize);
uint8_t getch(uint8_t wait);

char device_read_raw_sector(Blockdevice* dev, uint64_t sector, uint32_t counter, void* buffer);
char device_write_raw_sector(Blockdevice* dev, uint64_t sector, uint32_t counter, void* buffer);
char* dir(char* basepath);
char readFile(char* path,void *buffer);
uint64_t writeFile(char* path,void *buffer,uint64_t size);
uint64_t getFileSize(char* path);