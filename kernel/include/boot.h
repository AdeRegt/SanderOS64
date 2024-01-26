#ifndef _boot
#define _boot

#include <stdint.h>
#include "outint.h"

typedef struct {
    uint32_t                          Type;           // Field size is 32 bits followed by 32 bit pad
    uint32_t                          Pad;
    upointer_t                        PhysicalStart;  // Field size is 64 bits
    upointer_t                        VirtualStart;   // Field size is 64 bits
    upointer_t                        NumberOfPages;  // Field size is 64 bits
    upointer_t                        Attribute;      // Field size is 64 bits
} MemoryDescriptor;

typedef struct{
    MemoryDescriptor* mMap;
	upointer_t mMapSize;
	upointer_t mMapDescSize;
}MemoryInfo;

typedef struct {
	void* BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
    unsigned char strategy;
	unsigned long pointerX;
	unsigned long pointerY;
} GraphicsInfo;

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_Header;

typedef struct {
	PSF1_Header* psf1_Header;
	void* glyphBuffer;
} PSF1_Font;

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
#endif 