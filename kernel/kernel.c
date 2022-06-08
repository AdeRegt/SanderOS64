#include "include/paging.h"
#include "include/kernel.h"
#include "include/gdt.h"
#include "include/idt.h"

void kernel_main(BootInfo *gi){
    initialise_gdt_driver();
    set_graphics_info(gi->graphics_info);
    set_memory_info(gi->memory_info);
    initialise_graphics_driver();
    initialise_memory_driver();
    // initialise_idt_driver();
    initialise_paging_driver();
    k_printf("\n__hello world__\n");for(;;);
    // int* test = (int*)0x80000000000;
    // *test = 2;
    for(;;);
}