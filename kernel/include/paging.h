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

// __attribute__ ((aligned(0x1000)))

typedef struct{
    Page page[512];
}PageTable;

typedef struct{
    Page pagetables[512];
}PageDirectoryTable;

typedef struct{
    Page pagedirectorytables[512];
}PageDirectoryPointerTable;

typedef struct{
    Page pagedirectorypointertables[512];
}PageMapLevel4Table;

typedef struct{
    unsigned long long page_map_level_4_table_index;        // PDP_i
    unsigned long long page_directory_pointer_table_index;  // PD_i
    unsigned long long page_directory_table_index;          // PT_i
    unsigned long long page_table_index;                    // P_i
}PageLookupResult;

void initialise_paging_driver();