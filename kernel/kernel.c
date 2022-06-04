#include "include/graphics.h"

void kernel_main(GraphicsInfo *gi){
    unsigned int y = 50;
    unsigned int BBP = 4;
    for(unsigned int x = 0 ; x < gi->Width / 2 * BBP ; x++){
        *(unsigned int*)(x +(y*gi->PixelsPerScanLine*BBP) + gi->BaseAddress) = 0xFFFFFFFF;
    }
    for(;;);
}