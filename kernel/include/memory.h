#include <stdint.h>
#include "outint.h"
#include "sizet.h"
#include "boot.h"

#define MEMORY_MAP_SIZE 1024
#define PAGE_SIZE 0x1000

#ifdef __x86_64
#define MEMORY_AREA_PICK_RATE 1000000
#else
#define MEMORY_AREA_PICK_RATE 1000
#endif 

#ifndef NULL 
#define NULL (void*)0
#endif 

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
upointer_t getMaxAddress();