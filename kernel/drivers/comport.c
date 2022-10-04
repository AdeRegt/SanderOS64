#include "../include/comport.h"
#include "../include/ports.h"
#include "../include/graphics.h"

int initialise_spec_comport(uint16_t addr){
   outportb(addr + 1, 0x00);
   outportb(addr + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outportb(addr + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outportb(addr + 1, 0x00);    //                  (hi byte)
   outportb(addr + 3, 0x03);    // 8 bits, no parity, one stop bit
   outportb(addr + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outportb(addr + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outportb(addr + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outportb(addr + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(inportb(addr + 0) != 0xAE) {
      return 1;
   }
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outportb(addr + 4, 0x0F);
   return 0;
}

int com_is_transmit_empty(uint16_t addr) {
   return inportb(addr + 5) & 0x20;
}
 
void com_write_serial(uint16_t addr, char a) {
   while (com_is_transmit_empty(addr) == 0);
 
   outportb(addr,a);
}

void com_write_debug_serial(char a){
   com_write_serial(0x3f8, a);
}

uint8_t comisenabled = 0;

uint8_t is_com_enabled(){
   return comisenabled;
}

void initialise_comport(){
    k_printf("com: initialising COM ports...\n");
    if(initialise_spec_comport(0x3f8)==0){
      comisenabled = 1;
    }
}