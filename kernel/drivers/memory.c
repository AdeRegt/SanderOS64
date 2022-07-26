#include "../include/memory.h"
#include "../include/graphics.h"
#include "../include/paging.h"

uint8_t memorymap[MEMORY_MAP_SIZE];
uint8_t emptymap[MEMORY_MAP_SIZE];
uint64_t max_memory;
uint64_t free_memory;
uint64_t used_memory;

uint64_t free_memory_min;
uint64_t free_memory_max;

uint64_t empty_memory_min;
uint64_t empty_memory_max;

uint64_t getExmptyMax(){
    return empty_memory_max;
}

MemoryInfo *memory_info;

void set_memory_info(MemoryInfo *gi){
    memory_info = gi;
}

uint64_t getMaximumMemory(){
    return max_memory;
}

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void *memconcat(void *base1,void *base2,uint64_t newsize,uint64_t size_base1){
    char* nd = (char*) malloc(newsize);
    memset(nd,0,newsize);
    memcpy(nd,base1,size_base1);
    memcpy(nd + newsize,base2,newsize - size_base1);
    return nd;
}

void initialise_memory_driver(){
    uint64_t mMapEntries = memory_info->mMapSize / memory_info->mMapDescSize;
    max_memory = 0;
    free_memory = 0;
    used_memory = 0;
    uint64_t maxaddr = 0;
    for (int i = 0; i < mMapEntries; i++){
        MemoryDescriptor* desc = (MemoryDescriptor*)((uint64_t)memory_info->mMap + (i * memory_info->mMapDescSize));
        uint64_t size = desc->NumberOfPages * 4096 ;
        uint64_t msize = desc->PhysicalStart + size;
        if(msize>maxaddr){
            maxaddr = msize;
        }
        max_memory += size;
        if(desc->Type==7){
            if( size>1000000 && desc->PhysicalStart!=0 && free_memory_min==0 ){
                free_memory += size;
                free_memory_min = desc->PhysicalStart;
                free_memory_max = desc->PhysicalStart + size;
            }else if(size>1000000 && desc->PhysicalStart!=0 && empty_memory_min==0 ){
                empty_memory_min = desc->PhysicalStart;
                empty_memory_max = desc->PhysicalStart + size;
            }
        }else{
            used_memory += size;
        }
    }
    k_printf("Max-memory: %dKB Free-memory: %dKB Used-memory: %dKB \n",max_memory,free_memory,used_memory);
    k_printf("Free area starts from %x to %x with the size of %x \n",free_memory_min,free_memory_max,free_memory_max-free_memory_min);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
    k_printf("The kernel has a size of %x which are %x kernelpages it starts at %x to %x \n",kernelSize,kernelPages,_KernelStart,_KernelEnd);
    k_printf("Max addr of PC is %x \n",maxaddr);
}

void memset(void *start, unsigned char value, uint64_t num){
    for(uint64_t i = 0 ; i < num ; i++){
        *(unsigned char*)((uint64_t)start + i) = value;
    }
}

void memcpy(void *to, void *from, uint64_t num){
    for(uint64_t i = 0 ; i < num ; i++){
        *(unsigned char*)((uint64_t)to + i) = *(unsigned char*)((uint64_t)from + i);
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

char get_memory_map_bit(uint64_t index){
    uint64_t byteIndex = index/8;
    unsigned char bitIndex = index % 8;
    unsigned char bitIndexer = 0b10000000 >> bitIndex;
    return (memorymap[byteIndex] & bitIndexer) !=0;
}

void set_memory_map_bit(uint64_t index,char value){
    uint64_t byteIndex = index/8;
    unsigned char bitIndex = index % 8;
    unsigned char bitIndexer = 0b10000000 >> bitIndex;
    memorymap[byteIndex] &= ~bitIndexer;
    if (value){
        memorymap[byteIndex] |= bitIndexer;
    }
}

void *requestPage(){
    for(uint64_t i = 0 ; i < ((free_memory_max-free_memory_min)/PAGE_SIZE) ; i++){
        if(memorymap[i]==0){
            memorymap[i] = 1;
            return (void *)(free_memory_min+(i * PAGE_SIZE));
        }
    }
    k_printf("Out of memory!\n");for(;;);
}

void *requestBigPage(){
    for(uint64_t i = 0 ; i < ((free_memory_max-free_memory_min)/PAGE_SIZE) ; i++){
        if(memorymap[i]==0){
            if(((free_memory_min+(PAGE_SIZE*i))&0xFFFFF)==0){
                if(((free_memory_min+(PAGE_SIZE*i))%PAGE_GAP_SIZE)==0){
                    memorymap[i] = 1;
                    return (void *)(free_memory_min+(i * PAGE_SIZE));
                }
            }
        }
    }
    k_printf("Out of memory!\n");for(;;);
}

void freePage(void* memory){
    uint64_t calculation = (uint64_t) memory;
    calculation -= free_memory_min;
    calculation /= PAGE_SIZE;
    memorymap[calculation] = 0;
}

void *malloc(uint64_t size){
    for(uint64_t i = 0 ; i < ((empty_memory_max-empty_memory_min)/0x100) ; i++){
        if(emptymap[i]==0){
            emptymap[i] = 1;
            return (void *)(empty_memory_min+(i * 0x100));
        }
    }
    return 0;
}

void free(void* memory){
    uint64_t calculation = (uint64_t) memory;
    calculation -= empty_memory_min;
    calculation /= 0x100;
    emptymap[calculation] = 0;
}