#include "../../include/bootstrapping/GRUB/multiboot.h"
#include "../../include/kernel.h"

BootInfo stdbf;
GraphicsInfo gi;
MemoryInfo mi;

void kernel_grub_main(multiboot_info_t* mbi, uint32_t magic){

    gi.BaseAddress = (void*) mbi->framebuffer_addr;
    gi.PixelsPerScanLine = mbi->framebuffer_width;
    gi.BufferSize = 0;
    gi.Height = mbi->framebuffer_height;
    gi.Width = mbi->framebuffer_width;
    gi.strategy = 1;

    stdbf.font = 0;
    stdbf.graphics_info = (GraphicsInfo*)&gi;
    stdbf.memory_info = (MemoryInfo*)&mi;
    stdbf.rsdp = 0;
    kernel_main((BootInfo*)&stdbf);
}