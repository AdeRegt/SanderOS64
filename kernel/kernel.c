#include "include/graphics.h"
#include "include/memory.h"

void kernel_main(GraphicsInfo *gi){
    set_graphics_info(gi);
    initialise_graphics_driver();
    for(;;);
}