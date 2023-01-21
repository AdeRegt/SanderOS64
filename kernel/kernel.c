#include "include/paging.h"
#include "include/kernel.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/pci.h"
#include "include/timer.h"
#include "include/device.h"
#include "include/ethernet.h"
#include "include/multitasking.h"
#include "include/tty.h"
#include "include/comport.h"
#include "include/ps2.h"
#include "include/exec/program.h"
#include "include/exec/module.h"

BootInfo *bi;

BootInfo *getBootInfo(){
    return bi;
}

void __stack_chk_fail(){
    for(;;);
}

void __stack_chk_fail_local(){
    for(;;);
}

void kernel_main(BootInfo *gi){
    bi = gi;
    initialise_gdt_driver();
    set_graphics_info(gi->graphics_info);
    set_memory_info(gi->memory_info);
    initialise_graphics_driver();
    initialise_memory_driver();
    initialise_paging_driver();
    initialise_comport();
    initialise_idt_driver();
    initialise_multitasking_driver();
    initialise_pci_driver();
    #ifndef use_driver
        initialise_drivers_from_pci();
    #endif 
    initialise_ethernet();
    post_init_kernel();
}

void post_init_kernel(){
    char* filedir = dir("A:SANDEROS");
    if(!filedir){
        k_printf("Unable to detect a valid kernel FS!\n");
        for(;;);
    }
    #ifdef use_driver
        loadModule("A:SANDEROS/DRIVERS/PS2KEY.SYS",0);
        initialise_drivers_from_pci();
    #else
        initialise_ps2_driver();
    #endif 
    initialise_tty();
    k_printf("__end of kernel!\n");
    for(;;);
}
