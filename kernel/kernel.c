#include "include/graphics.h"
#include "include/psf.h"

void kernel_main(GraphicsInfo *gi){
    set_graphics_info(gi);
    clear_screen(0xFFFFFFFF);

    setActiveFont(getDefaultFont());
    k_printf("Hallo wereld! %x %d %s %c \nabc",10,10,"inputstring",'!');
    for(;;);
}