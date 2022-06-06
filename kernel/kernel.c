#include "include/paging.h"
#include "include/kernel.h"

void kernel_main(BootInfo *gi){
    set_graphics_info(gi->graphics_info);
    set_memory_info(gi->memory_info);
    initialise_graphics_driver();
    initialise_memory_driver();
    initialise_paging_driver();
    for(;;);
}