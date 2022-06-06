#include "../include/memory.h"
#include "../include/graphics.h"

unsigned long memorymap[MEMORY_MAP_SIZE];

void initialise_memory_driver(){
    
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