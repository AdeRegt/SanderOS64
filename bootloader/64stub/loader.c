#include <stdint.h>
#include "include/graphics.h"

void __stack_chk_fail(){
    for(;;);
}

void __stack_chk_fail_local(){
    for(;;);
}

void main(){
    GraphicsInfo c0;
    GraphicsInfo *c1 = (GraphicsInfo*) &c0;

    c1->BaseAddress = (void*) 0xD0000000;
    c1->Width = 0x320;
    c1->Height = 0x258;
    c1->PixelsPerScanLine = 0x320;
    c1->strategy = 1;

    set_graphics_info(c1);

    initialise_graphics_driver();
    // k_printf("!!!!!!!!!!!!!!!!!!!!!\n");
    for(;;);
}