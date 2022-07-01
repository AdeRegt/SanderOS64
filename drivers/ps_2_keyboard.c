#include "driver.h"

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64

__attribute__((interrupt)) void keyboardirq(interrupt_frame* frame){
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
    k_printf("keyboard is typed!\n");
}

void driver_start(PCIInfo *pci){
    k_printf("This is the keyboarddriver!\n");
    while(inportb(PS2_STATUS) & 0b00000010);
    outportb(PS2_DATA,0xF4);
    while(inportb(PS2_DATA)!=0xFA);
    setInterrupt(1,keyboardirq);
}