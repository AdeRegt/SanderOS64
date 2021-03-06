#include "include/paging.h"
#include "include/kernel.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/pci.h"
#include "include/timer.h"
#include "include/device.h"
#include "include/multitasking.h"
#include "include/ps2.h"
#include "include/tty.h"
#include "include/exec/program.h"
#include "include/exec/module.h"

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
    initialise_multitasking_driver();
    initialise_pci_driver();
    char* filedir = dir("A:SANDEROS");
    if(!filedir){
        k_printf("Unable to detect a valid kernel FS!\n");
        for(;;);
    }
    initialise_drivers_from_pci();
    initialise_ps2_driver();
    initialise_tty();
    k_printf("__end of kernel!\n");
    for(;;);
}