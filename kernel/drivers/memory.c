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

extern unsigned long long _KernelStart;
extern unsigned long long _KernelEnd;

void initialise_memory_driver(){
    unsigned long long mMapEntries = memory_info->mMapSize / memory_info->mMapDescSize;
    max_memory = 0;
    free_memory = 0;
    used_memory = 0;
    for (int i = 0; i < mMapEntries; i++){
        MemoryDescriptor* desc = (MemoryDescriptor*)((unsigned long long)memory_info->mMap + (i * memory_info->mMapDescSize));
        unsigned long long size = desc->NumberOfPages * 4096 ;
        max_memory += size;
        if(desc->Type==7){
            if( size>1000000 && desc->PhysicalStart!=0 ){
                free_memory += size;
                free_memory_min = desc->PhysicalStart;
                free_memory_max = desc->PhysicalStart + size;
                break;
            }
        }else{
            used_memory += size;
        }
    }
    k_printf("Max-memory: %dKB Free-memory: %dKB Used-memory: %dKB \n",max_memory,free_memory,used_memory);
    k_printf("Free area starts from %x to %x with the size of %x \n",free_memory_min,free_memory_max,free_memory_max-free_memory_min);

    unsigned long long kernelSize = (unsigned long long)&_KernelEnd - (unsigned long long)&_KernelStart;
    unsigned long long kernelPages = (unsigned long long)kernelSize / 4096 + 1;
    k_printf("The kernel has a size of %x which are %x kernelpages it starts at %x to %x \n",kernelSize,kernelPages,_KernelStart,_KernelEnd);
}

void memset(void *start, unsigned char value, unsigned long long num){
    for(unsigned long long i = 0 ; i < num ; i++){
        *(unsigned char*)((unsigned long long)start + i) = value;
    }
}

void memcpy(void *to, void *from, unsigned long long num){
    for(unsigned long long i = 0 ; i < num ; i++){
        *(unsigned char*)((unsigned long long)to + i) = *(unsigned char*)((unsigned long long)from + i);
    }
}

uint64_t strcmp(uint8_t* a, uint8_t* b, uint64_t length){
	for (uint64_t i = 0; i < length; i++){
		if (*a != *b) return 0;
	}
	return 1;
}

uint64_t strlen(uint8_t* message){
    uint64_t res = 0;
    uint8_t this = 1;
    while(this){
        this = message[res++];
    }
    return res;
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
    for(unsigned long long i = 0 ; i < ((free_memory_max-free_memory_min)/0x1000) ; i++){
        if(memorymap[i]==0){
            memorymap[i] = 1;
            return (void *)(free_memory_min+(i * 0x1000));
        }
    }
    k_printf("Out of memory!\n");for(;;);
}

void freePage(void* memory){
    uint64_t calculation = (uint64_t) memory;
    calculation -= free_memory_min;
    calculation /= 0x1000;
    memorymap[calculation] = 0;
}