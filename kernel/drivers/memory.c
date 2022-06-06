#include "../include/memory.h"
#include "../include/graphics.h"

unsigned long memorymap[MEMORY_MAP_SIZE];
unsigned long long max_memory;
unsigned long long free_memory;
unsigned long long used_memory;

MemoryInfo *memory_info;

void set_memory_info(MemoryInfo *gi){
    memory_info = gi;
}

void initialise_memory_driver(){
    unsigned long long mMapEntries = memory_info->mMapSize / memory_info->mMapDescSize;
    max_memory = 0;
    free_memory = 0;
    used_memory = 0;
    for (int i = 0; i < mMapEntries; i++){
        MemoryDescriptor* desc = (MemoryDescriptor*)((unsigned long long)memory_info->mMap + (i * memory_info->mMapDescSize));
        max_memory += desc->NumberOfPages * 4096 / 1024;
        if(desc->Type==7){
            free_memory += desc->NumberOfPages * 4096 / 1024;
        }else{
            used_memory += desc->NumberOfPages * 4096 / 1024;
        }
    }
    k_printf("Max-memory: %dKB Free-memory: %dKB Used-memory: %dKB \n",max_memory,free_memory,used_memory);
    for(;;);
}

void memset(void *start, unsigned char value, unsigned long long num){
    for(unsigned long long i = 0 ; i < num ; i++){
        *(unsigned char*)((unsigned long long)start + i) = value;
    }
}

char get_memory_map_bit(unsigned long long index){
    unsigned long long byteIndex = index/8;
    unsigned char bitIndex = index % 8;
    unsigned char bitIndexer = 0b10000000 >> bitIndex;
    return (memorymap[byteIndex] & bitIndexer) !=0;
}

void set_memory_map_bit(unsigned long long index,char value){
    unsigned long long byteIndex = index/8;
    unsigned char bitIndex = index % 8;
    unsigned char bitIndexer = 0b10000000 >> bitIndex;
    memorymap[byteIndex] &= ~bitIndexer;
    if (value){
        memorymap[byteIndex] |= bitIndexer;
    }
}

void *requestPage(){
    for(unsigned long long i = 0 ; i < MEMORY_MAP_SIZE ; i++){
        if(memorymap[i]==0){
            memorymap[i] = 1;
            return (void *)(i * (unsigned long long)(0x1000));
        }
    }
    k_printf("Out of memory!\n");for(;;);
}