#include "../include/memory.h"
#include "../include/graphics.h"
#include "../include/paging.h"

uint8_t memorymap[MEMORY_MAP_SIZE];
uint8_t emptymap[MEMORY_MAP_SIZE];
upointer_t max_memory;
upointer_t free_memory;
upointer_t used_memory;

upointer_t free_memory_min;
upointer_t free_memory_max;

upointer_t empty_memory_min;
upointer_t empty_memory_max;

upointer_t getExmptyMax(){
    return empty_memory_max;
}

MemoryInfo *memory_info;

void set_memory_info(MemoryInfo *gi){
    memory_info = gi;
}

upointer_t getMaximumMemory(){
    return max_memory;
}

extern upointer_t _KernelStart;
extern upointer_t _KernelEnd;

void *memconcat(void *base1,void *base2,upointer_t newsize,upointer_t size_base1){
    char* nd = (char*) malloc(newsize);
    memset(nd,0,newsize);
    memcpy(nd,base1,size_base1);
    memcpy(nd + newsize,base2,newsize - size_base1);
    return nd;
}

int memcmp(const void* aptr, const void* bptr, size_t n){
	const unsigned char* a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++){
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}

void initialise_memory_driver(){
    k_printf("memory: mMapSize:%d mMapDescSize:%d entries:%d\n",memory_info->mMapSize , memory_info->mMapDescSize,memory_info->mMapSize / memory_info->mMapDescSize);
    upointer_t mMapEntries = memory_info->mMapSize / memory_info->mMapDescSize;
    max_memory = 0;
    free_memory = 0;
    used_memory = 0;
    free_memory_min = 0;
    empty_memory_min = 0;
    upointer_t maxaddr = 0;
    for (int i = 0; i < mMapEntries; i++){
        MemoryDescriptor* desc = (MemoryDescriptor*)((upointer_t)memory_info->mMap + (i * memory_info->mMapDescSize));
        upointer_t size = desc->NumberOfPages * 4096 ;
        upointer_t msize = desc->PhysicalStart + size;
        if(msize>maxaddr){
            maxaddr = msize;
        }
        max_memory += size;
        // k_printf("type: %d mem:%x size=%x \n",desc->Type,desc->PhysicalStart,size);
        if(desc->Type==7){
            if( size>MEMORY_AREA_PICK_RATE && desc->PhysicalStart!=0 && free_memory_min==0 ){
                free_memory += size;
                free_memory_min = desc->PhysicalStart;
                free_memory_max = desc->PhysicalStart + size;
            }else if(size>MEMORY_AREA_PICK_RATE && desc->PhysicalStart!=0 && empty_memory_min==0 ){
                empty_memory_min = desc->PhysicalStart;
                empty_memory_max = desc->PhysicalStart + size;
            }
        }else{
            used_memory += size;
        }
    }
    k_printf("Max-memory: %dKB Free-memory: %dKB Used-memory: %dKB \n",max_memory,free_memory,used_memory);
    k_printf("Free area starts from %x to %x with the size of %x \n",free_memory_min,free_memory_max,free_memory_max-free_memory_min);
    k_printf("Empty area starts from %x to %x with the size of %x \n",empty_memory_min,empty_memory_max,empty_memory_max-empty_memory_min);
    // for(;;);

    upointer_t kernelSize = (upointer_t)&_KernelEnd - (upointer_t)&_KernelStart;
    upointer_t kernelPages = (upointer_t)kernelSize / 4096 + 1;
    k_printf("The kernel has a size of %x which are %x kernelpages it starts at %x to %x \n",kernelSize,kernelPages,_KernelStart,_KernelEnd);
    k_printf("Max addr of PC is %x \n",maxaddr);
}

void memset(void *start, unsigned char value, upointer_t num){
    for(upointer_t i = 0 ; i < num ; i++){
        *(unsigned char*)((upointer_t)start + i) = value;
    }
}

void memcpy(void *to, void *from, upointer_t num){
    for(upointer_t i = 0 ; i < num ; i++){
        *(unsigned char*)((upointer_t)to + i) = *(unsigned char*)((upointer_t)from + i);
    }
}

upointer_t strcmp(uint8_t* a, uint8_t* b, upointer_t length){
	for (upointer_t i = 0; i < length; i++){
		if (*a != *b) return 0;
	}
	return 1;
}

upointer_t strlen(uint8_t* message){
    upointer_t res = 0;
    uint8_t this = 1;
    while(this){
        this = message[res++];
    }
    return res;
}

char get_memory_map_bit(upointer_t index){
    upointer_t byteIndex = index/8;
    unsigned char bitIndex = index % 8;
    unsigned char bitIndexer = 0b10000000 >> bitIndex;
    return (memorymap[byteIndex] & bitIndexer) !=0;
}

void set_memory_map_bit(upointer_t index,char value){
    upointer_t byteIndex = index/8;
    unsigned char bitIndex = index % 8;
    unsigned char bitIndexer = 0b10000000 >> bitIndex;
    memorymap[byteIndex] &= ~bitIndexer;
    if (value){
        memorymap[byteIndex] |= bitIndexer;
    }
}

void *requestPage(){
    for(upointer_t i = 0 ; i < ((free_memory_max-free_memory_min)/PAGE_SIZE) ; i++){
        if(memorymap[i]==0){
            memorymap[i] = 1;
            return (void *)(free_memory_min+(i * PAGE_SIZE));
        }
    }
    k_printf("Out of memory!\n");for(;;);
}

void *requestBigPage(){
    for(upointer_t i = 0 ; i < ((free_memory_max-free_memory_min)/PAGE_SIZE) ; i++){
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
    upointer_t calculation = (upointer_t) memory;
    calculation -= free_memory_min;
    calculation /= PAGE_SIZE;
    memorymap[calculation] = 0;
}

void *malloc(upointer_t size){
    for(upointer_t i = 0 ; i < ((empty_memory_max-empty_memory_min)/0x100) ; i++){
        if(emptymap[i]==0){
            emptymap[i] = 1;
            return (void *)(empty_memory_min+(i * 0x100));
        }
    }
    return 0;
}

void free(void* memory){
    upointer_t calculation = (upointer_t) memory;
    calculation -= empty_memory_min;
    calculation /= 0x100;
    emptymap[calculation] = 0;
}