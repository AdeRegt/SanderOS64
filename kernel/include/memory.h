#include <stdint.h>

#define MEMORY_MAP_SIZE 1024

typedef struct {
    uint32_t                          Type;           // Field size is 32 bits followed by 32 bit pad
    uint32_t                          Pad;
    uint64_t                          PhysicalStart;  // Field size is 64 bits
    uint64_t                          VirtualStart;   // Field size is 64 bits
    uint64_t                          NumberOfPages;  // Field size is 64 bits
    uint64_t                          Attribute;      // Field size is 64 bits
} MemoryDescriptor;

typedef struct{
    MemoryDescriptor* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
}MemoryInfo;

void initialise_memory_driver();
void memset(void *start, unsigned char value, unsigned long long num);
void *requestPage();
void set_memory_info(MemoryInfo *gi);
unsigned long long getMaximumMemory();