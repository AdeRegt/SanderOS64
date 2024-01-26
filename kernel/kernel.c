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
#include "include/bmp.h"
#include "include/winman.h"

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
    initialise_idt_driver();
    set_graphics_info(gi->graphics_info);
    set_memory_info(gi->memory_info);
    initialise_graphics_driver();
    initialise_memory_driver();
    initialise_paging_driver();
    initialise_comport();
    initialise_multitasking_driver();
    initialise_pci_driver();
    #ifndef use_driver
        initialise_drivers_from_pci();
        initialise_ps2_driver();
    #endif 
    initialise_ethernet();
    char* filedir = dir("A:SANDEROS");
    if(!filedir){
        k_printf("Unable to detect a valid kernel FS!\n");
        for(;;);
    }
    #ifdef use_driver
        loadModule("A:SANDEROS/DRIVERS/PS2KEY.SYS",0);
        initialise_drivers_from_pci();
    #endif 
    clear_screen(0xFF0000FF);
    draw_bmp_from_file("A:SANDEROS/SPLASH.BMP",10,10);
    sleep(200);
    window_manager_create_confirm_box("How are you?");
    initialise_tty();
    halt("__end of kernel!\n");
}
