#pragma once 
#include <stdint.h>
#include "ethernet.h"

//
// FROM: https://wiki.osdev.org/RTL8169
// This is a entry in the queue of the recieve and transmit descriptor queue
#define OWN 0x80000000
#define EOR 0x40000000
struct Descriptor{
	uint32_t command;  /* command/status uint32_t */
	uint32_t vlan;     /* currently unused */
	void *buffer;
};

void init_rtl(int bus,int slot,int function);