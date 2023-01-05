#ifndef __x86_64
    #include <stdint.h>
    #include "../include/bootstrapping/GRUB/multiboot.h"
    #include "../include/kernel.h"

    void ep_grub(multiboot_info_t *grub, uint32_t magic){
        
        BootInfo b0;
        BootInfo *b1 = (BootInfo*) &b0;

        GraphicsInfo c0;
        GraphicsInfo *c1 = (GraphicsInfo*) &c0;

        MemoryInfo d0;
        MemoryInfo *d1 = (MemoryInfo*) &d0;

        c1->BaseAddress = (void*) (uint32_t)grub->framebuffer_addr;
        c1->Width = grub->framebuffer_width;
        c1->Height = grub->framebuffer_height;
        c1->PixelsPerScanLine = c1->Width;
        c1->strategy = 1;

        MemoryDescriptor db[10];
        memset(&db,0,sizeof(MemoryDescriptor)*10);
        int z = 0;
        for(int i = 0; i < grub->mmap_length;  i += sizeof(multiboot_memory_map_t)) {
            multiboot_memory_map_t* mmmt =  (multiboot_memory_map_t*) (grub->mmap_addr + i);
    
            if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
               db[z].Type=7;
               db[z].NumberOfPages = mmmt->len_low / 4096;
               db[z].PhysicalStart = mmmt->addr_low;
               db[z].VirtualStart = mmmt->addr_low;
               z++;
            }
            
        }
        d1->mMapDescSize = sizeof(MemoryDescriptor);
        d1->mMapSize = d1->mMapDescSize*z;
        d1->mMap = (MemoryDescriptor*)&db;

        b1->graphics_info = c1;
        b1->memory_info = d1;

        kernel_main(b1);
        for(;;);
    }
#endif