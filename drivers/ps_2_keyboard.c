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

uint8_t getPS2StatusRegisterText(){
	return inportb(PS2_STATUS);
}

int getPS2ReadyToRead(){
	return getPS2StatusRegisterText() & 0b00000001;
}

int getPS2ReadyToWrite(){
	return getPS2StatusRegisterText() & 0b00000010;
}

int writeToFirstPS2Port(uint8_t data){
	while(getPS2ReadyToWrite()>0);
	outportb(PS2_DATA,data);
	return 1;
}

int writeToSecondPS2Port(uint8_t data){
	outportb(0x64,0xD4);
	while(getPS2ReadyToWrite()>0);
	outportb(0x60,data);
	return 1;
}

int waitforps2ok(){
	while(inportb(PS2_DATA)!=0xFA);
	return 1;
}

int ps2_echo(){
	int a = writeToFirstPS2Port(0xEE);
	if(a==0){
		return 0;
	}
	while(inportb(PS2_DATA)!=0xEE);
	return 1;
}

uint8_t mousewaita(){
	unsigned long timeout = 100000;
	while(--timeout){
		if((inportb(0x64) & 0x01)==1){
			return 1;
		}
	}
    return 0;
}

uint8_t mousewaitb(){
	unsigned long timeout = 100000;
	while(--timeout){
		if(!(inportb(0x64) & 0x02)){
			return 1;
		}
	}
    return 0;
}

static uint8_t last_pressed_key = 0;

__attribute__((interrupt)) void ps2_keyboard__interrupt(interrupt_frame* frame){
    uint8_t pressed_key = inportb(PS2_DATA);
    if((pressed_key&0x80)==0){
        last_pressed_key = kbdus[pressed_key];
    }
    outportb(0x20,0x20);
}

__attribute__((interrupt)) void ps2_mouse__interrupt(interrupt_frame* frame){
    uint8_t status = inportb(PS2_STATUS);
    if(status & 1){
        uint8_t pressed_key = inportb(PS2_DATA);
        if(status & 0x20){
            k_printf("Mouse interrupt!\n");
        }
    }
    outportb(0x20,0x20);
}

uint8_t getch(uint8_t wait){
    if(wait){
        while(last_pressed_key==0);
    }
    uint8_t tmp = last_pressed_key;
    last_pressed_key = 0;
    return tmp;
}

uint8_t initialise_ps2_keyboard_driver(){
    if(!writeToFirstPS2Port(0xF4)){goto error;}
	if(!waitforps2ok()){goto error;}
	
    setInterrupt(1,ps2_keyboard__interrupt);
    return 1;
    
    error:
    return 0;
}

uint8_t initialise_ps2_mouse_driver(){
    if(!mousewaita()){goto error;}
	outportb(0x64,0xA8);
	if(!mousewaita()){goto error;}
	outportb(0x64,0x20);
	if(!mousewaitb()){goto error;}
	unsigned char status =  inportb(0x60);
	inportb(0x60);
	status |= (1 << 1);
	status &= ~(1 << 5);
	if(!mousewaita()){goto error;}
	outportb(0x64,0x60);
	if(!mousewaita()){goto error;}
	outportb(0x60,status);
	
	if(!writeToSecondPS2Port(0xF6)){goto error;}
	if(!waitforps2ok()){goto error;}
	if(!writeToSecondPS2Port(0xF4)){goto error;}
	if(!waitforps2ok()){goto error;}
	
    setInterrupt(12,ps2_mouse__interrupt);
    return 1;
    
    error:
    return 0;
}

void driver_start(PCIInfo *pci){
    k_printf("ps2: trying to initialise ps2\n");
    if(ps2_echo()){
        k_printf("ps2: echo as expected!\n");
        // do we have a ps2 mouse pressent?
        if(initialise_ps2_mouse_driver()){
            k_printf("ps2: mouse detected!\n");
        }else{
            k_printf("ps2: failed to load mouse!\n");
        }
        // lets initialise keyboard!
        if(initialise_ps2_keyboard_driver()){
            k_printf("ps2: keyboard detected!\n");
        }else{
            k_printf("ps2: failed to load keyboard!\n");
        }
    }else{
        k_printf("ps2: no echo!\n");
    }
}