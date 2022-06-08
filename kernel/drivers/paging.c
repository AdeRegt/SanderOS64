#include "../include/paging.h"
#include "../include/graphics.h"
#include "../include/memory.h"

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

void dumpPageInfo(Page pageinfo){
    k_printf("-pageinfo: AX:%x AD:%x AV:%x CD:%x LP:%x PR:%x RW:%x SU:%x WT:%x \n",pageinfo.accessed,(uint64_t)pageinfo.address<<12,pageinfo.available,pageinfo.cachedisabled,pageinfo.largepages,pageinfo.present,pageinfo.readwrite,pageinfo.usersuper,pageinfo.writetrough);
}

void map_memory(void *virtualmemory,void* physicalmemory){
    // k_printf("Mapping page %x to %x \n",physicalmemory,virtualmemory);
    PageLookupResult lookup = page_map_indexer((unsigned long long)virtualmemory);
    PageTable* PLM4 = (PageTable*) pagemaplevel4;
    Page PDE = PLM4->pages[lookup.page_map_level_4_table_index];
    PageTable *PDP;
    if(!PDE.present){
        PDP = (PageTable*) requestPage();
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
        PD = (PageTable*) requestPage();
        memset(PD, 0, 0x1000);
        PDE.address = (unsigned long long)PD >> 12;
        PDE.present = 1;
        PDE.readwrite = 1;
        PDP->pages[lookup.page_directory_pointer_table_index] = PDE;
    }else{
        PD = (PageTable*)((unsigned long long)PDE.address<<12);
    }

    PDE = PD->pages[lookup.page_directory_table_index];
    PDE.address     = (unsigned long long)physicalmemory >> 12;
    PDE.present     = 1;
    PDE.readwrite   = 1;
    PDE.largepages  = 1;
    PD->pages[lookup.page_directory_table_index] = PDE;
}


void initialise_paging_driver(){
    k_printf("At default, the pagemaplevel4 is located at %x \n",pagemaplevel4);
    uint64_t cr0, cr2, cr3;
    __asm__ __volatile__ (
        "mov %%cr0, %%rax\n\t"
        "mov %%eax, %0\n\t"
        "mov %%cr2, %%rax\n\t"
        "mov %%eax, %1\n\t"
        "mov %%cr3, %%rax\n\t"
        "mov %%eax, %2\n\t"
    : "=m" (cr0), "=m" (cr2), "=m" (cr3)
    : /* no input */
    : "%rax"
    );
    k_printf("CR1=%x CR2=%x CR3=%x \n",cr0,cr2,cr3);
    k_printf("Attaching CR3 value as location of pagemaptable4\n");
    pagemaplevel4 = (PageTable*) cr3;
    k_printf("Now setting the CPU register\n");
    asm ("mov %0, %%cr3" : : "r" (pagemaplevel4));
    k_printf("When we hit this point, we are safe!\n");
    k_printf("EOF\n");
    for(;;);
}