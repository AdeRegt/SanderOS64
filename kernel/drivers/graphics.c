#include "../include/graphics.h"

GraphicsInfo *graphics_info;

void set_graphics_info(GraphicsInfo *gi){
    graphics_info = gi;
}

void draw_pixel_at(unsigned int x,unsigned int y,unsigned int colour){
    unsigned int BBP = 4;
    *(unsigned int*)((x*BBP) +(y*graphics_info->PixelsPerScanLine*BBP) + graphics_info->BaseAddress) = colour;
}

void clear_screen(unsigned int colour){
    unsigned int BBP = 4;
    for(unsigned int y = 0 ; y < graphics_info->Height ; y++){
        for(unsigned int x = 0 ; x < graphics_info->Width ; x++){
            draw_pixel_at(x,y,colour);
        }
    }
}