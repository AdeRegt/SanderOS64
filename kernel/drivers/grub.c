#ifndef __x86_64
    #include <stdint.h>
    #include "../include/bootstrapping/GRUB/multiboot.h"
    #include "../include/kernel.h"

    void ep_grub(multiboot_info_t *grub, uint32_t magic){
        
        BootInfo b0;
        BootInfo *b1 = (BootInfo*) &b0;

        GraphicsInfo c0;
        GraphicsInfo *c1 = (GraphicsInfo*) &c0;

        c1->BaseAddress = (void*) (uint32_t)grub->framebuffer_addr;
        c1->Width = grub->framebuffer_width;
        c1->Height = grub->framebuffer_height;
        c1->PixelsPerScanLine = c1->Width;
        c1->strategy = 1;

        b1->graphics_info = c1;
        kernel_main(b1);
        for(;;);
    }
#endif