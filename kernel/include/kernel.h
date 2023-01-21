#include "graphics.h"
#include "psf.h"
#include "memory.h"

/**
 * @brief Bootinfo structure given by the program that booted us
 * 
 */
typedef struct{

	/**
	 * @brief Graphicsinfo from the current device, like screensize, bytes per pixel, etc
	 * 
	 */
	GraphicsInfo* graphics_info;

	/**
	 * @brief Fontfile that is used by the bootloader
	 * 
	 */
	PSF1_Font* font;

	/**
	 * @brief Memoryinfo with memorymap of the free memory which is present
	 * 
	 */
	MemoryInfo* memory_info;

	/**
	 * @brief Information table of which devices are present on this system
	 * 
	 */
	void *rsdp;
} BootInfo;

/**
 * @brief Main entry point of the program
 * 
 * @param gi Bootinfo given by the program which booted us
 */
void kernel_main(BootInfo *gi);

/**
 * @brief Get the Boot Info object
 * 
 * @return BootInfo* 
 */
BootInfo *getBootInfo();

void post_init_kernel();