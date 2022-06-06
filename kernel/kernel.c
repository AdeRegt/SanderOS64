#include "include/paging.h"
#include "include/kernel.h"
#include "include/gdt.h"

void kernel_main(BootInfo *gi){
    initialise_gdt_driver();
    set_graphics_info(gi->graphics_info);
    set_memory_info(gi->memory_info);
    initialise_graphics_driver();
    initialise_memory_driver();
    // initialise_paging_driver();
    for(;;);
}