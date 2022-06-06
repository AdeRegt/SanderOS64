#include "../include/memory.h"
#include "../include/graphics.h"

unsigned long memorymap[MEMORY_MAP_SIZE];
unsigned long long max_memory;
unsigned long long free_memory;
unsigned long long used_memory;

unsigned long long free_memory_min;
unsigned long long free_memory_max;

MemoryInfo *memory_info;

void set_memory_info(MemoryInfo *gi){
    memory_info = gi;
}

unsigned long long getMaximumMemory(){
    return max_memory;
}

void initialise_memory_driver(){
    unsigned long long mMapEntries = memory_info->mMapSize / memory_info->mMapDescSize;
    max_memory = 0;
    free_memory = 0;
    used_memory = 0;
    for (int i = 0; i < mMapEntries; i++){
        MemoryDescriptor* desc = (MemoryDescriptor*)((unsigned long long)memory_info->mMap + (i * memory_info->mMapDescSize));
        unsigned long long size = desc->NumberOfPages * 4096 / 1024;
        max_memory += size;
        if(desc->Type==7){
            int z = desc->PhysicalStart;
            if( size>1000000 && z!=0 ){
                free_memory += size;
                free_memory_min = desc->PhysicalStart;
                free_memory_max = desc->PhysicalStart + size;
            }
        }else{
            used_memory += size;
        }
    }
    k_printf("Max-memory: %dKB Free-memory: %dKB Used-memory: %dKB \n",max_memory,free_memory,used_memory);
    k_printf("Free area starts from %x to %x with the size of %x \n",free_memory_min,free_memory_max,free_memory_max-free_memory_min);
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
    for(unsigned long long i = 0 ; i < (MEMORY_MAP_SIZE) ; i++){
        if(memorymap[i]==0){
            memorymap[i] = 1;
            return (void *)(i * 0x1000);
        }
    }
    k_printf("Out of memory!\n");for(;;);
}