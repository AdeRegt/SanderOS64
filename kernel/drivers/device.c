#include "../include/device.h"
#include "../include/memory.h"
#include "../include/graphics.h"
#include "../include/exec/debugger.h"

Blockdevice blockdevices[10];
int blockdevicemaxpointer = 0;
Filesystem filesystems[10];
int filesystemmaxpointer = 0;

Blockdevice* getFreeBlockDevice(){
    Blockdevice* dev = (Blockdevice*) &blockdevices[blockdevicemaxpointer++];
    return dev;
}

Blockdevice* registerBlockDevice(uint64_t blocksize, void* readcommand, void* writecommand, uint64_t offset, void* attachment){
    Blockdevice* dev = getFreeBlockDevice();
    dev->blocksize = blocksize;
    dev->readcommand = readcommand;
    dev->writecommand = writecommand;
    dev->offset = offset;
    dev->attachment = attachment;
    return dev;
}

char device_read_raw_sector(Blockdevice* dev, uint64_t sector, uint32_t counter, void* buffer){
    unsigned char (*foo)(Blockdevice*, uint64_t, uint32_t, void*) = (void*)dev->readcommand;
	return foo(dev,dev->offset + sector,counter,buffer);
}

char device_write_raw_sector(Blockdevice* dev, uint64_t sector, uint32_t counter, void* buffer){
    unsigned char (*foo)(Blockdevice*, uint64_t, uint32_t, void*) = (void*)dev->writecommand;
	return foo(dev,dev->offset + sector,counter,buffer);
}

Filesystem* getFreeFilesystem(){
    Filesystem* dev = (Filesystem*) &filesystems[filesystemmaxpointer++];
    return dev;
}

Filesystem* registerFileSystem(Blockdevice *bd,void *read,void *dir,void *filesize,void *write){
    Filesystem* fs = getFreeFilesystem();
    fs->blockdevice = bd;
    fs->readfile = read;
    fs->dir = dir;
    fs->filesize = filesize;
    fs->write = write;
    return fs;
}

char* dir(char* basepath){
    if(basepath[0]>='a'&&basepath[0]<='z'){
        basepath[0] += ('A' - 'a');
    }
    char driveletter = basepath[0] - 'A';
    Filesystem fs = filesystems[driveletter];
    if(fs.dir){
        char* (*foo)(Filesystem*, char*) = (void*)fs.dir;
        basepath++;
        basepath++;
	    return foo((Filesystem*)&fs,basepath);
    }
    return 0;
}

char readFile(char* path,void *buffer){
    if(path[0]>='a'&&path[0]<='z'){
        path[0] += ('A' - 'a');
    }
    char driveletter = path[0] - 'A';
    Filesystem fs = filesystems[driveletter];
    if(fs.readfile){
        char (*foo)(Filesystem*, char*, void*) = (void*)fs.readfile;
        path++;
        path++;
	    return foo((Filesystem*)&fs,path,buffer);
    }
    return 0;
}

uint64_t writeFile(char* path,void *buffer,uint64_t size){
    if(path[0]>='a'&&path[0]<='z'){
        path[0] += ('A' - 'a');
    }
    char driveletter = path[0] - 'A';
    Filesystem fs = filesystems[driveletter];
    if(fs.readfile){
        uint64_t (*foo)(Filesystem*, char*, void*, uint64_t) = (void*)fs.write;
        path++;
        path++;
	    return foo((Filesystem*)&fs,path,buffer,size);
    }
    return 0;
}

uint64_t getFileSize(char* path){
    if(path[0]>='a'&&path[0]<='z'){
        path[0] += ('A' - 'a');
    }
    char driveletter = path[0] - 'A';
    Filesystem fs = filesystems[driveletter];
    if(fs.filesize){
        uint64_t (*foo)(Filesystem*, char*) = (void*)fs.filesize;
        path++;
        path++;
	    return foo((Filesystem*)&fs,path);
    }
    return 0;
}

uint8_t *scanLine(uint64_t maxsize){
    uint8_t *result = (uint8_t*) malloc(maxsize);
    memset(result,0,maxsize);
    for(uint64_t i = 0 ; i < maxsize ; i++){
        uint8_t r = getch(1);
        if(r=='\n'){
            break;
        }
        putc(r);
        result[i] = r;
    } 
    return result;
}

void *getcpoint;

uint8_t getch(uint8_t wait){
    uint8_t (*foo)(uint8_t) = (void*)getcpoint;
    return foo(wait);
}

void registerHIDDevice(void *getcpointer){
    k_printf("Registering HID at %x \n",getcpointer);
    getcpoint = getcpointer;
}