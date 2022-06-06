#include "../include/paging.h"
#include "../include/graphics.h"
#include "../include/memory.h"

PageMapLevel4Table pagemaplevel4 __attribute__ ((aligned(0x1000)));

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

void map_memory(void *virtualmemory,void* physicalmemory){
    PageLookupResult lookup = page_map_indexer((unsigned long long)virtualmemory);
    PageMapLevel4Table* PLM4 = (PageMapLevel4Table*) &pagemaplevel4;
    Page PDE = PLM4->pagedirectorypointertables[lookup.page_map_level_4_table_index];
    PageDirectoryPointerTable *PDP;
    if(!PDE.present){
        PDP = (PageDirectoryPointerTable*) requestPage();
        memset(PDP, 0, 0x1000);
        PDE.address = (unsigned long long)PDP >> 12;
        PDE.present = 1;
        PDE.readwrite = 1;
        PLM4->pagedirectorypointertables[lookup.page_map_level_4_table_index] = PDE;
    }else{
        PDP = (PageDirectoryPointerTable*)((unsigned long long)PDE.address<<12);
    }

    PDE = PDP->pagedirectorytables[lookup.page_directory_pointer_table_index];
    PageDirectoryTable *PD;
    if(!PDE.present){
        PD = (PageDirectoryTable*) requestPage();
        memset(PD, 0, 0x1000);
        PDE.address = (unsigned long long)PD >> 12;
        PDE.present = 1;
        PDE.readwrite = 1;
        PDP->pagedirectorytables[lookup.page_directory_pointer_table_index] = PDE;
    }else{
        PD = (PageDirectoryTable*)((unsigned long long)PDE.address<<12);
    }

    PDE = PD->pagetables[lookup.page_directory_pointer_table_index];
    PageTable *PT;
    if(!PDE.present){
        PT = (PageTable*) requestPage();
        memset(PT, 0, 0x1000);
        PDE.address = (unsigned long long)PT >> 12;
        PDE.present = 1;
        PDE.readwrite = 1;
        PD->pagetables[lookup.page_directory_pointer_table_index] = PDE;
    }else{
        PT = (PageTable*)((unsigned long long)PDE.address<<12);
    }

    PDE = PT->page[lookup.page_table_index];
    PDE.address = (unsigned long long)physicalmemory >> 12;
    PDE.present = 1;
    PDE.readwrite = 1;
    PT->page[lookup.page_table_index] = PDE;
}

void initialise_paging_driver(){
    k_printf("Aplying memory map...\n");
    for(unsigned long long i = 0 ; i < 0x100000 ; i++){
        map_memory((void *)(i*0x1000),(void *)(i*0x1000));
    }
    k_printf("Now setting the CPU register\n");
    // asm ("mov %0, %%cr3" : : "r" (&pagemaplevel4));
    k_printf("When we hit this point, we are safe!\n");
}