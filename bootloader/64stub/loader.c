#include <stdint.h>
#include "include/graphics.h"
#include "../../kernel/include/outint.h"

void __stack_chk_fail(){
    for(;;);
}

void __stack_chk_fail_local(){
    for(;;);
}

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

typedef struct{
    Page pages[512];
}PageTable;

typedef struct{
    upointer_t page_map_level_4_table_index;        // PDP_i
    upointer_t page_directory_pointer_table_index;  // PD_i
    upointer_t page_directory_table_index;          // PT_i
    upointer_t page_table_index;                    // P_i
}PageLookupResult;

#define PAGE_GAP_SIZE 0x200000

PageTable *pagemaplevel4;

PageLookupResult page_map_indexer(unsigned long long virtual_address){
    PageLookupResult plr;
    virtual_address >>= 12;
    plr.page_table_index = virtual_address & 0x1ff;
    virtual_address >>= 9;
    plr.page_directory_table_index = virtual_address & 0x1ff;
    virtual_address >>= 9;
    plr.page_directory_pointer_table_index = virtual_address & 0x1ff;
    virtual_address >>= 9;
    plr.page_map_level_4_table_index = virtual_address & 0x1ff;
    return plr;
}

void *memreg = (void*)0x200000;

void *memcpy(void *dst, const void *src, size_t len){
    char *d = dst;
    const char *s = src;
    for (size_t i=0; i<len; i++) {
        d[i] = s[i];
    }
    return dst;
}
void *memset(void *dst, uint8_t val, size_t len){
    char *d = dst;
    for (size_t i=0; i<len; i++) {
        d[i] = val;
    }
    return dst;
}

void map_memory(void* pml4mem, void *virtualmemory,void* physicalmemory){
    // k_printf("Mapping page %x to %x \n",physicalmemory,virtualmemory);
    PageLookupResult lookup = page_map_indexer((unsigned long long)virtualmemory);
    PageTable* PLM4 = (PageTable*) pml4mem;
    Page PDE = PLM4->pages[lookup.page_map_level_4_table_index];
    PageTable *PDP;
    if(!PDE.present){
        PDP = (PageTable*) memreg;
        memreg += 0x1000;
        memset(PDP, 0, 0x1000);
        PDE.address = (unsigned long long)PDP >> 12;
        PDE.present = 1;
        PDE.readwrite = 1;
        PLM4->pages[lookup.page_map_level_4_table_index] = PDE;
    }else{
        PDP = (PageTable*)((unsigned long long)PDE.address<<12);
    }

    PDE = PDP->pages[lookup.page_directory_pointer_table_index];
    PageTable *PD;
    if(!PDE.present){
        PD = (PageTable*) memreg;
        memreg += 0x1000;
        memset(PD, 0, 0x1000);
        PDE.address = (unsigned long long)PD >> 12;
        PDE.present = 1;
        PDE.readwrite = 1;
        PDP->pages[lookup.page_directory_pointer_table_index] = PDE;
    }else{
        PD = (PageTable*)((unsigned long long)PDE.address<<12);
    }

    PDE = PD->pages[lookup.page_directory_table_index];
    PageTable *PE;
    PDE.address     = (unsigned long long)physicalmemory>>12;//PE >> 12;
    PDE.present     = 1;
    PDE.readwrite   = 1;
    PDE.largepages  = 1;
    PD->pages[lookup.page_directory_table_index] = PDE;
}


void initialise_paging_driver(){

    pagemaplevel4 = (void*) 0x100000;
    memset(pagemaplevel4,0,sizeof(PageTable));
    for(upointer_t valve = 0 ; valve < (0xFFFFF000/PAGE_GAP_SIZE) ; valve++){
        map_memory(pagemaplevel4,(void*)(valve*PAGE_GAP_SIZE),(void*)(valve*PAGE_GAP_SIZE));
    }
    asm volatile ("mov %0, %%cr3" : : "r" (pagemaplevel4));
    
}

void main(){

    GraphicsInfo c0;
    GraphicsInfo *c1 = (GraphicsInfo*) &c0;

    c1->BaseAddress = (void*) 0xD0000000;
    c1->Width = 0x320;
    c1->Height = 0x258;
    c1->PixelsPerScanLine = 0x320;
    c1->strategy = 1;
    set_graphics_info(c1);

    initialise_paging_driver();

    initialise_graphics_driver();
    k_printf("!!!!!!!!!!!!!!!!!!!!!\n");
    for(;;);
}