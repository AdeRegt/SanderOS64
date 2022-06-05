#include "include/graphics.h"
#include "include/psf.h"

void kernel_main(GraphicsInfo *gi){
    set_graphics_info(gi);
    clear_screen(0xFFFFFFFF);

    setActiveFont(getDefaultFont());
    drawCharacter(getActiveFont(),'@',0x00000000,50,50);
    for(;;);
}