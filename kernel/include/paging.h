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
    unsigned long long address: 52;
}Page;

typedef struct{
    Page pages[512];
}PageTable;

typedef struct{
    unsigned long long page_map_level_4_table_index;        // PDP_i
    unsigned long long page_directory_pointer_table_index;  // PD_i
    unsigned long long page_directory_table_index;          // PT_i
    unsigned long long page_table_index;                    // P_i
}PageLookupResult;

#define EXTERNAL_PROGRAM_ADDRESS 0xC00000

void initialise_paging_driver();
void map_memory(void* pml4mem, void *virtualmemory,void* physicalmemory);