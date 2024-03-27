#include "../include/paging.h"
#include "../include/graphics.h"
#include "../include/memory.h"

PageTable *pagemaplevel4;

void *getPagingTable(){
    return pagemaplevel4;
}

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

#ifdef __x86_64
void dumpPageInfo(Page pageinfo){
    k_printf("-pageinfo: AX:%x AD:%x AV:%x CD:%x LP:%x PR:%x RW:%x SU:%x WT:%x \n",pageinfo.accessed,(uint64_t)pageinfo.address<<12,pageinfo.available,pageinfo.cachedisabled,pageinfo.largepages,pageinfo.present,pageinfo.readwrite,pageinfo.usersuper,pageinfo.writetrough);
}
#endif

void *memreg = (void*)0x100000;

void *getMappedMemoryFor(void* pml4mem,void *virtualmemory){
    #ifdef __x86_64
    PageLookupResult lookup = page_map_indexer((unsigned long long)virtualmemory);
    PageTable* PLM4 = (PageTable*) pml4mem;
    Page PDE = PLM4->pages[lookup.page_map_level_4_table_index];
    PageTable *PDP;
    if(!PDE.present){
        return 0;
    }else{
        PDP = (PageTable*)((unsigned long long)PDE.address<<12);
    }

    PDE = PDP->pages[lookup.page_directory_pointer_table_index];
    PageTable *PD;
    if(!PDE.present){
        return 0;
    }else{
        PD = (PageTable*)((unsigned long long)PDE.address<<12);
    }

    PDE = PD->pages[lookup.page_directory_table_index];
    PageTable *PE;
    return (void*) (upointer_t)(PDE.address<<12);
    #endif 
    return 0;
}

void map_memory(void* pml4mem, void *virtualmemory,void* physicalmemory){
    #ifdef __x86_64
        // k_printf("Mapping page %x to %x \n",physicalmemory,virtualmemory);
        PageLookupResult lookup = page_map_indexer((unsigned long long)virtualmemory);
        PageTable* PLM4 = (PageTable*) pml4mem;
        Page PDE = PLM4->pages[lookup.page_map_level_4_table_index];
        PageTable *PDP;
        if(!PDE.present){
            PDP = (PageTable*) requestPage();
            memset(PDP, 0, PAGE_SIZE);
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
            memset(PD, 0, PAGE_SIZE);
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
    #endif
}


void initialise_paging_driver(){

    pagemaplevel4 = requestPage();
    memset(pagemaplevel4,0,PAGE_SIZE);
    for(upointer_t valve = 0 ; valve < (0xFFFFF000/PAGE_GAP_SIZE)-1 ; valve++){
        map_memory(pagemaplevel4,(void*)(valve*PAGE_GAP_SIZE),(void*)(valve*PAGE_GAP_SIZE));
    }
    k_printf("All pages are filled now load it to the cr3 register!\n");
    asm volatile ("mov %0, %%cr3" : : "r" (pagemaplevel4));
    k_printf("When we hit this point, we are safe! (new method)\n");
    
}