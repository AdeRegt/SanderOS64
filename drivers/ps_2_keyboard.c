#include "driver.h"

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64

#define VK_UP 0xCB
#define VK_LEFT 0xCC
#define VK_RIGHT 0xCD
#define VK_DOWN 0xCE
#define VK_SHIFT 0xCF

uint8_t kbdus[128] ={
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   VK_SHIFT,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   VK_SHIFT,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    VK_UP,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    VK_LEFT,	/* Left Arrow */
    0,
    VK_RIGHT,	/* Right Arrow */
  '+',
    1,	/* 79 - End key*/
    VK_DOWN,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};	

static uint8_t last_pressed_key = 0;

__attribute__((interrupt)) void ps2_keyboard__interrupt(interrupt_frame* frame){
    uint8_t pressed_key = inportb(PS2_DATA);
    if((pressed_key&0x80)==0){
        last_pressed_key = kbdus[pressed_key];
    }
    outportb(0x20,0x20);
}

uint8_t get2ch(uint8_t wait){
    if(wait){
        while(last_pressed_key==0);
    }
    uint8_t tmp = last_pressed_key;
    last_pressed_key = 0;
    return tmp;
}

void driver_start(PCIInfo *pci){
    k_printf("ps2: trying to initialise ps2\n");
    while((inportb(PS2_STATUS) & 0b00000010));
    outportb(PS2_DATA,0xF4);
	  while(inportb(PS2_DATA)!=0xFA);
	
    registerHIDDevice(get2ch);
    setInterrupt(1,ps2_keyboard__interrupt);
}