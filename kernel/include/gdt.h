#include <stdint.h>
#include "outint.h"

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

void initialise_gdt_driver();
extern void LoadGDT(GDTDescriptor* gdtDescriptor);