#include "include/paging.h"
#include "include/kernel.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/pci.h"
#include "include/timer.h"

BootInfo *bi;

BootInfo *getBootInfo(){
    return bi;
}

void kernel_main(BootInfo *gi){
    bi = gi;
    initialise_gdt_driver();
    set_graphics_info(gi->graphics_info);
    set_memory_info(gi->memory_info);
    initialise_graphics_driver();
    initialise_memory_driver();
    initialise_paging_driver();
    initialise_idt_driver();
    initialise_timer_driver();
    initialise_pci_driver();
    for(;;);
}