#include "../../include/bootstrapping/GRUB/multiboot.h"
#include "../../include/kernel.h"

BootInfo stdbf;
GraphicsInfo gi;
MemoryInfo mi;

MemoryDescriptor md[10];

void kernel_grub_main(multiboot_info_t* mbi, uint32_t magic){

    gi.BaseAddress = (void*) (uint32_t) mbi->framebuffer_addr;
    gi.PixelsPerScanLine = mbi->framebuffer_width;
    gi.BufferSize = 0;
    gi.Height = mbi->framebuffer_height;
    gi.Width = mbi->framebuffer_width;
    gi.strategy = 1;

    multiboot_memory_map_t *memmap = (multiboot_memory_map_t*) mbi->mmap_addr;
    int pointerA = 0;
    for(multiboot_uint32_t i = 0 ; i < mbi->mmap_length; i++){
        multiboot_memory_map_t thisone = (multiboot_memory_map_t)memmap[i];
        if( thisone.type==MULTIBOOT_MEMORY_AVAILABLE ){
            md[pointerA].Type = 7;
            md[pointerA].PhysicalStart = thisone.addr;
            md[pointerA].NumberOfPages = thisone.size;
            pointerA++;
        }
    }

    mi.mMap = (MemoryDescriptor*)&md;
    mi.mMapDescSize = sizeof(MemoryDescriptor);
    mi.mMapSize = 10 * sizeof(MemoryDescriptor);

    stdbf.font = 0;
    stdbf.graphics_info = (GraphicsInfo*)&gi;
    stdbf.memory_info = (MemoryInfo*)&mi;
    stdbf.rsdp = 0;
    kernel_main((BootInfo*)&stdbf);
}