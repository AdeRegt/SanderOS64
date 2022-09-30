#include <stdint.h>
#include "outint.h"

#define MEMORY_MAP_SIZE 1024
#define PAGE_SIZE 0x1000

typedef unsigned long long size_t;

typedef struct {
    uint32_t                          Type;           // Field size is 32 bits followed by 32 bit pad
    uint32_t                          Pad;
    upointer_t                        PhysicalStart;  // Field size is 64 bits
    upointer_t                        VirtualStart;   // Field size is 64 bits
    upointer_t                        NumberOfPages;  // Field size is 64 bits
    upointer_t                        Attribute;      // Field size is 64 bits
} MemoryDescriptor;

typedef struct{
    MemoryDescriptor* mMap;
	upointer_t mMapSize;
	upointer_t mMapDescSize;
}MemoryInfo;

void initialise_memory_driver();
void memset(void *start, unsigned char value, upointer_t num);
upointer_t strcmp(uint8_t* a, uint8_t* b, upointer_t length);
int memcmp(const void* aptr, const void* bptr, size_t n);
void *memconcat(void *base1,void *base2,upointer_t newsize,upointer_t size_base1);
void *requestPage();
void *requestBigPage();
void freePage(void* memory);
void set_memory_info(MemoryInfo *gi);
upointer_t getMaximumMemory();
void memcpy(void *to, void *from, upointer_t num);
upointer_t strlen(uint8_t* message);

void *malloc(upointer_t size);
void free(void* memory);