#include "graphics.h"
#include "psf.h"
#include "memory.h"
#include "boot.h"

#define halt(x) k_printf(x);for(;;);

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