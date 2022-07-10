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

void *memreg = (void*)0xF00000;

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
    if(!PDE.present){
        PE = (PageTable*) memreg;
        memreg += 0x1000;
        memset(PE, 0, 0x1000);
        PDE.address     = (unsigned long long)physicalmemory>>12;//PE >> 12;
        PDE.present     = 1;
        PDE.readwrite   = 1;
        PDE.largepages  = 1;
        PD->pages[lookup.page_directory_table_index] = PDE;
    }else{
        PE = (PageTable*)((unsigned long long)PDE.address<<12);
    }

    // Page *PDX = (Page*) &PE->pages[lookup.page_table_index];
    // PDX->address     = (unsigned long long)physicalmemory >> 12;
    // PDX->present     = 1;
    // PDX->readwrite   = 1;
}


void initialise_paging_driver(){

    pagemaplevel4 = requestPage();
    for(uint64_t valve = 0 ; valve < (0xFFFFF000/0x200000) ; valve++){
        map_memory(pagemaplevel4,(void*)(valve*0x200000),(void*)(valve*0x200000));
    }
    asm volatile ("mov %0, %%cr3" : : "r" (pagemaplevel4));
    k_printf("When we hit this point, we are safe! (new method)\n");

    //
    // Pre-enable pages!
    // void *dff = requestPage();

    // PageLookupResult plr = page_map_indexer(dff);
    // // map_memory((void*)addr,dff);
    // PageTable* PLM4 = (PageTable*) pagemaplevel4;
    // Page PDE = PLM4->pages[plr.page_map_level_4_table_index];
    // dumpPageInfo(PDE);
    // PageTable *PDP = (PageTable*)((unsigned long long)PDE.address<<12);
    // PDE = PDP->pages[plr.page_directory_pointer_table_index];
    // dumpPageInfo(PDE);
    // // ((uint8_t*)EXTERNAL_PROGRAM_ADDRESS)[1] = 'z';
    // PageTable *PD = (PageTable*)((unsigned long long)PDE.address<<12);
    // PDE = PD->pages[plr.page_directory_table_index];
    // dumpPageInfo(PDE);
    // Page glu = PD->pages[plr.page_directory_table_index];
    // glu.present = 0;
    // PD->pages[plr.page_directory_table_index] = glu;

    // plr = page_map_indexer(EXTERNAL_PROGRAM_ADDRESS);
    // // map_memory((void*)addr,dff);
    // PLM4 = (PageTable*) pagemaplevel4;
    // PDE = PLM4->pages[plr.page_map_level_4_table_index];
    // dumpPageInfo(PDE);
    // PDP = (PageTable*)((unsigned long long)PDE.address<<12);
    // PDE = PDP->pages[plr.page_directory_pointer_table_index];
    // dumpPageInfo(PDE);
    // // ((uint8_t*)EXTERNAL_PROGRAM_ADDRESS)[1] = 'z';
    // PD = (PageTable*)((unsigned long long)PDE.address<<12);
    // PDE = PD->pages[plr.page_directory_table_index];
    // dumpPageInfo(PDE);
    // glu = PD->pages[plr.page_directory_table_index];
    // glu.address = ((uint64_t)dff)>>12;
    // glu.accessed = 0;
    // glu.present = 1;
    // PD->pages[plr.page_directory_table_index] = glu;
    // ((uint8_t*)EXTERNAL_PROGRAM_ADDRESS)[0] ='z';
    // PDE = PD->pages[plr.page_directory_table_index];
    // dumpPageInfo(PDE);

    // k_printf("--> %d %d %d [%d] \n",plr.page_map_level_4_table_index,plr.page_directory_pointer_table_index,plr.page_directory_table_index,plr.page_table_index);
    // for(;;);
}