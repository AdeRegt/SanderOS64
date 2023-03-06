#include "../include/bmp.h"
#include "../include/graphics.h"

/*
 * Renders a bmp file into the screen.
 * NOTE: For now just render the entire image in the screen
 * (should be the same size as the window)
*/
void draw_bmp(unsigned char* file_buffer, unsigned short offsetX, unsigned short offsetY) {
	// read file_buffer and extract the pixel_buffer
	BMP_HEADER *header = (BMP_HEADER*) file_buffer;
    if(!(header->signature[0]=='B'&&header->signature[1]=='M')){
        k_printf("bmp: invalid BM ignature\n");
        return;
    }
    if(header->bits_per_pixel!=32){
        k_printf("bmp: invalid bits per pixel 32");
        return;
    }
    void *pixel_buffer = (void*) (file_buffer + header->offset_to_pixelarray);
    uint32_t* xxx = (uint32_t*) pixel_buffer;
    // if(header->compression){
    //     k_printf("bmp: compression\n");
    //     return;
    // }

	// draw the image
	for(uint32_t x = 0 ; x < header->width ; x++){
		for(uint32_t y = 4 ; y < header->height ; y++){
            unsigned int a = xxx[(y*header->width)+x];

            // k_printf("%x ",a);
			draw_pixel_at(offsetX + x, offsetY + y, a);
		}
	}
}