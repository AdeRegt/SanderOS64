#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


/* Type for a 16-bit quantity.  */
typedef uint16_t Elf32_Half;
typedef uint16_t Elf64_Half;

/* Types for signed and unsigned 32-bit quantities.  */
typedef uint32_t Elf32_Word;
typedef	int32_t  Elf32_Sword;
typedef uint32_t Elf64_Word;
typedef	int32_t  Elf64_Sword;

/* Types for signed and unsigned 64-bit quantities.  */
typedef uint64_t Elf32_Xword;
typedef	int64_t  Elf32_Sxword;
typedef uint64_t Elf64_Xword;
typedef	int64_t  Elf64_Sxword;

/* Type of addresses.  */
typedef uint32_t Elf32_Addr;
typedef uint64_t Elf64_Addr;

/* Type of file offsets.  */
typedef uint32_t Elf32_Off;
typedef uint64_t Elf64_Off;

/* Type for section indices, which are 16-bit quantities.  */
typedef uint16_t Elf32_Section;
typedef uint16_t Elf64_Section;

/* Type for version symbol information.  */
typedef Elf32_Half Elf32_Versym;
typedef Elf64_Half Elf64_Versym;

typedef struct
{
  Elf64_Word	p_type;			/* Segment type */
  Elf64_Word	p_flags;		/* Segment flags */
  Elf64_Off	p_offset;		/* Segment file offset */
  Elf64_Addr	p_vaddr;		/* Segment virtual address */
  Elf64_Addr	p_paddr;		/* Segment physical address */
  Elf64_Xword	p_filesz;		/* Segment size in file */
  Elf64_Xword	p_memsz;		/* Segment size in memory */
  Elf64_Xword	p_align;		/* Segment alignment */
} Elf64_Phdr;

#define EI_NIDENT (16)
typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
  Elf64_Half	e_type;			/* Object file type */
  Elf64_Half	e_machine;		/* Architecture */
  Elf64_Word	e_version;		/* Object file version */
  Elf64_Addr	e_entry;		/* Entry point virtual address */
  Elf64_Off	e_phoff;		/* Program header table file offset */
  Elf64_Off	e_shoff;		/* Section header table file offset */
  Elf64_Word	e_flags;		/* Processor-specific flags */
  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
  Elf64_Half	e_phentsize;		/* Program header table entry size */
  Elf64_Half	e_phnum;		/* Program header table entry count */
  Elf64_Half	e_shentsize;		/* Section header table entry size */
  Elf64_Half	e_shnum;		/* Section header table entry count */
  Elf64_Half	e_shstrndx;		/* Section header string table index */
} Elf64_Ehdr;

typedef struct
{
  Elf64_Word sh_name; /* Section name */
  Elf64_Word sh_type; /* Section type */
  Elf64_Word sh_flagsA; /* Section attributes */
  Elf64_Word sh_flagsB; /* Section attributes */
  Elf32_Addr sh_addrA; /* Virtual address in memory */
  Elf32_Addr sh_addrB; /* Virtual address in memory */
  Elf32_Off sh_offsetA; /* Offset in file */
  Elf32_Off sh_offsetB; /* Offset in file */
  Elf64_Word sh_sizeA; /* Size of section */
  Elf64_Word sh_sizeB; /* Size of section */
  Elf64_Word sh_link; /* Link to other section */
  Elf64_Word sh_info; /* Miscellaneous information */
  Elf64_Xword sh_addralign; /* Address alignment boundary */
  Elf64_Xword sh_entsize; /* Size of entries, if section has table */
} Elf64_Shdr;

typedef struct
{
  Elf64_Addr r_offset; /* Address of reference */
  Elf64_Xword r_info; /* Symbol index and type of relocation */
} Elf64_Rel;

typedef struct
{
  Elf64_Addr r_offset; /* Address of reference */
  Elf64_Xword r_info; /* Symbol index and type of relocation */
  Elf64_Sxword r_addend; /* Constant part of expression */
} Elf64_Rela;

typedef struct
{
  Elf64_Word st_name; /* Symbol name */
  unsigned char st_info; /* Type and Binding attributes */
  unsigned char st_other; /* Reserved */
  Elf64_Half st_shndx; /* Section table index */
  Elf64_Addr st_value; /* Symbol value */
  Elf64_Xword st_size; /* Size of object (e.g., common) */
} Elf64_Sym;

#define PT_LOAD		1		/* Loadable program segment */
#define	ELFMAG		"\177ELF"
#define	SELFMAG		4
#define EI_MAG0		0		/* File identification byte 0 index */
#define SHF_ALLOC 0x2

#define ELF64_R_SYM(i)((i) >> 32)
#define ELF64_R_TYPE(i)((i) & 0xffffffffL)
#define ELF64_R_INFO(s, t)(((s) << 32) + ((t) & 0xffffffffL))

typedef struct
{
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint32_t vbe_mode;
	uint32_t vbe_interface_seg;
	uint32_t vbe_interface_off;
	uint32_t vbe_interface_len;
}GRUBMultiboot ;

typedef struct{
  /* the memory used goes from bytes ’mod_start’ to ’mod_end-1’ inclusive */
  uint32_t mod_start;
  uint32_t mod_end;

  /* Module command line */
  uint32_t cmdline;

  /* padding to take it to 16 bytes (must be zero) */
  uint32_t pad;
}multiboot_mod_list;
 
/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

typedef struct {
	uint64_t BaseAddress;
	uint64_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
    uint8_t strategy;
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

typedef struct {
    uint32_t                          Type;           // Field size is 32 bits followed by 32 bit pad
    uint32_t                          Pad1;
    uint64_t                          PhysicalStart;  // Field size is 64 bits
    uint64_t                          VirtualStart;   // Field size is 64 bits
    uint64_t                          NumberOfPages;  // Field size is 64 bits
    uint64_t                          Attribute;      // Field size is 64 bits
    uint8_t                           Pad2[8];
} MemoryDescriptor;

typedef struct{
    uint64_t mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
}MemoryInfo;

GraphicsInfo graphicsinfo;
MemoryInfo memoryinfo;
MemoryDescriptor memdesclist[15];


typedef struct{

	/**
	 * @brief Graphicsinfo from the current device, like screensize, bytes per pixel, etc
	 * 
	 */
	uint64_t graphics_info;

	/**
	 * @brief Fontfile that is used by the bootloader
	 * 
	 */
	uint64_t font;

	/**
	 * @brief Memoryinfo with memorymap of the free memory which is present
	 * 
	 */
	uint64_t memory_info;

	/**
	 * @brief Information table of which devices are present on this system
	 * 
	 */
	uint64_t *rsdp;
} BootInfo;

typedef struct 
{
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    #define MULTIBOOT_MEMORY_AVAILABLE              1
    #define MULTIBOOT_MEMORY_RESERVED               2
    #define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
    #define MULTIBOOT_MEMORY_NVS                    4
    #define MULTIBOOT_MEMORY_BADRAM                 5
    uint32_t type;
}__attribute__((packed)) multiboot_mmap_entry;

BootInfo bootinfo;

 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
    if(c=='\n'){
        terminal_row++;
        terminal_column = 0;
        return;
    }
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void nolongmodemessager()
{
    terminal_writestring("No long mode available\n");
}

char *convert(unsigned int num, int base) { 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 

    for(int i = 0 ; i < 50 ; i++){
        buffer[i] = 0x00;
    }

	if(num==0){
		ptr = &buffer[0];
		buffer[0] = '0';
		return ptr;
	}
	
	ptr = &buffer[49]; 
	*--ptr = '\0'; 
	*--ptr = '\0'; 
	*--ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0);
	
	return(ptr); 
}

typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

void printf(char* format,...){
    va_list arg; 
	va_start(arg, format);
    int length = 0;
    while(1){
        char deze = format[length];
        if(deze=='\0'){
            break;
        }else if(deze=='%'){
            length++;
            deze = format[length];
            if(deze=='c'){
                char i = va_arg(arg,int);
                terminal_putchar(i);
            }else if(deze=='%'){
                terminal_putchar('%');
            }else if(deze=='s'){
                char *s = va_arg(arg,char *);
                terminal_writestring(s);
            }else if(deze=='x'){
                int t = va_arg(arg,unsigned int);
                terminal_putchar('0');
                terminal_putchar('x');
                char *convertednumber = convert(t,16);
                terminal_writestring(convertednumber);
            }else if(deze=='d'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,10);
                terminal_writestring(convertednumber);
            }else if(deze=='o'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,8);
                terminal_writestring(convertednumber);
            }
            length++;
        }else{
            terminal_putchar(deze);
            length++;
        }
    }
    va_end(arg);
}

uint64_t epoint;

extern void *_BootEnd;
 
void kernel_main(GRUBMultiboot *grub, uint32_t magic) 
{
	/* Initialize terminal interface */
	terminal_initialize();
 
	/* Newline support is left as an exercise. */
	terminal_writestring("Bootloader\n");
    if(magic!=0x2BADB002){
        terminal_writestring("Invalid header\n");
        return;
    }
    if(grub->mods_count!=1){
        terminal_writestring("mods_count!=1\n");
        return;
    }
    multiboot_mod_list* modlist = (multiboot_mod_list*) grub->mods_addr;
    void* dat = (void*) modlist->mod_start;
    
    Elf64_Ehdr* header = (Elf64_Ehdr*) dat;
    epoint = header->e_entry;

    printf("e_shnum: %d e_shentsize: %d \n",header->e_shnum,header->e_shentsize);
    for(Elf64_Half i = 0 ; i < header->e_shnum ; i++){
        Elf64_Shdr* shr = (Elf64_Shdr*) (header->e_shoff + dat + (header->e_shentsize*i));
        if(shr->sh_flagsA&2){
            printf("%d: flags: %x address: %x offset: %x size: %x \n",i,shr->sh_flagsA,shr->sh_addrA,shr->sh_offsetA,shr->sh_sizeA);
            for(uint32_t z = 0 ; z < shr->sh_sizeA; z++){
                ((uint8_t*)(shr->sh_addrA))[z] = ((uint8_t*)(shr->sh_offsetA + dat))[z];
            }
        }
    }

    // printf("mmap length: %d mmap addr: %x \n",grub->mmap_length,grub->mmap_addr);for(;;);
    memdesclist[0].Type = 7;
    memdesclist[0].NumberOfPages = 300;
    memdesclist[0].PhysicalStart = (uint64_t)((uint32_t)_BootEnd);

    memdesclist[1].Type = 7;
    memdesclist[1].NumberOfPages = 300;
    memdesclist[1].PhysicalStart = ((uint64_t)((uint32_t)_BootEnd)) + ( memdesclist[0].NumberOfPages * 4096 );

    bootinfo.memory_info = (uint64_t)((uint32_t)&memoryinfo);
    memoryinfo.mMapDescSize = sizeof(MemoryDescriptor);
    memoryinfo.mMapSize = 15 * memoryinfo.mMapDescSize;
    memoryinfo.mMap = (uint64_t)((uint32_t)&memdesclist);

    bootinfo.graphics_info = (uint64_t)((uint32_t)&graphicsinfo);
    graphicsinfo.BaseAddress = 0xB8000;
    graphicsinfo.BufferSize = 3145728;
    graphicsinfo.Height = 24;
    graphicsinfo.Width = 160;
    graphicsinfo.PixelsPerScanLine = 160;
    graphicsinfo.strategy = 2;

    bootinfo.rsdp = 0;

    bootinfo.font = 0;

}