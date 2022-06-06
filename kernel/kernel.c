#include "include/graphics.h"
#include "include/memory.h"
#include "include/paging.h"
#include "include/psf.h"
#include "include/kernel.h"

void kernel_main(BootInfo *gi){
    set_graphics_info(gi->graphics_info);
    initialise_graphics_driver();
    k_printf("Welcome to SanderOS64!\n");
    for(;;);
    initialise_memory_driver();
    initialise_paging_driver();
    for(;;);
}