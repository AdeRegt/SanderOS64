OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
	. = 0x1000;
	.text : ALIGN(0x1000)
	{
		*(.multiboot)
		*(.text)
	}
	.data :
	{
		*(.data)
	}
	.rodata : 
	{
		*(.rodata)
	}
	.bss : 
	{
		*(COMMON)
		*(.bss)
	}
}