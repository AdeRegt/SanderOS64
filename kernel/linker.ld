/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	.text : ALIGN(0x1000)
	{
		*(.text)
	}
 
	/* Read-write data (initialized) */
	.data : ALIGN(0x1000)
	{
		*(.data)
	}
 
	/* Read-only data. */
	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}
 
}