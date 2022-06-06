#include "graphics.h"
#include "psf.h"
#include "memory.h"

typedef struct{
	GraphicsInfo* graphics_info;
	PSF1_Font* font;
	MemoryInfo* memory_info;
} BootInfo;

void kernel_main(BootInfo *gi);