#include <stdint.h>
#include "outint.h"

#ifdef __x86_64
typedef struct  __attribute__((packed)){
    uint16_t Size;
    upointer_t Offset;
}GDTDescriptor;

typedef struct __attribute__((packed)) {
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t AccessByte;
    uint8_t Limit1_Flags;
    uint8_t Base2;
} GDTEntry;

typedef struct __attribute__((packed)) __attribute((aligned(0x1000))) {
    GDTEntry Null; //0x00
    GDTEntry KernelCode; //0x08
    GDTEntry KernelData; //0x10
    GDTEntry UserNull;
    GDTEntry UserCode;
    GDTEntry UserData;
} GDT;
extern void LoadGDT(GDTDescriptor* gdtDescriptor);
#else 
struct gdt_entry{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));
#endif 

void initialise_gdt_driver();