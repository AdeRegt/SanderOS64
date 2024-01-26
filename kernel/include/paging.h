
#include "outint.h"

#ifdef __x86_64
typedef struct{
    unsigned char present: 1;
    unsigned char readwrite: 1;
    unsigned char usersuper: 1;
    unsigned char writetrough: 1;
    unsigned char cachedisabled: 1;
    unsigned char accessed: 1;
    unsigned char ignore1: 1;
    unsigned char largepages: 1;
    unsigned char ignore2: 1;
    unsigned char available: 3;
    upointer_t address: 52;
}Page;
#endif

#ifndef __x86_64
typedef struct{}Page;
#endif 

typedef struct{
    Page pages[512];
}PageTable;

typedef struct{
    upointer_t page_map_level_4_table_index;        // PDP_i
    upointer_t page_directory_pointer_table_index;  // PD_i
    upointer_t page_directory_table_index;          // PT_i
    upointer_t page_table_index;                    // P_i
}PageLookupResult;

#define EXTERNAL_PROGRAM_ADDRESS 0xC000000
#define PAGE_GAP_SIZE 0x200000

void initialise_paging_driver();
void map_memory(void* pml4mem, void *virtualmemory,void* physicalmemory);
void unmap_memory(void* pml4mem, void *virtualmemory);
Page *getPageOf(void* pml4mem, void *virtualmemory);
void *getPagingTable();