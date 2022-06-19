#include "../include/pci.h"
#include "../include/kernel.h"
#include "../include/ports.h"
#include "../include/ahci.h"
#include "../include/xhci.h"

unsigned short pciConfigReadWord (unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset) {
    unsigned long address;
    unsigned long lbus  = (unsigned long)bus;
    unsigned long lslot = (unsigned long)slot;
    unsigned long lfunc = (unsigned long)func;
    unsigned short tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (unsigned long)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((unsigned long)0x80000000));
 
    /* write out the address */
    outportl(PCI_ADDRESS, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (unsigned short)((inportl(PCI_DATA) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}


unsigned long getBARaddress(int bus,int slot,int function,int barNO){
	unsigned long result = 0;
	unsigned long partA = pciConfigReadWord(bus,slot,function,barNO);
	unsigned long partB = pciConfigReadWord(bus,slot,function,barNO+2);
	result = ((partB<<16) | ((partA) & 0xffff));
	return result;
}

void initialise_pci_driver(){
    k_printf("Scanning for diskdriver to find bootdevice...\n");
    // scan for usb device
	for(int bus = 0 ; bus < 256 ; bus++){
		for(int slot = 0 ; slot < 32 ; slot++){
			for(int function = 0 ; function <= 7 ; function++){
				unsigned short vendor = pciConfigReadWord(bus,slot,function,0);
				if(vendor != 0xFFFF){
					unsigned char classc = (pciConfigReadWord(bus,slot,function,0x0A)>>8)&0xFF;
					unsigned char sublca = (pciConfigReadWord(bus,slot,function,0x0A))&0xFF;
                    #ifdef BOOT_WITH_AHCI
                    if(classc==0x01&&sublca==0x06){
	                    unsigned long bar5 = getBARaddress(bus,slot,function,0x24);
                        unsigned long usbint = getBARaddress(bus,slot,function,0x3C) & 0x000000FF;
                        initialise_ahci_driver(bar5,usbint);
                    }
                    #endif
                    #ifdef BOOT_WITH_XHCI
					unsigned char subsub = (pciConfigReadWord(bus,slot,function,0x08)>>8)&0xFF;
                    if(classc==0x0C&&sublca==0x03&&subsub==0x30){
                        // this is a xhci driver! we can do this...
                        k_printf("xhci device found...\n");
	                    unsigned long bar0 = getBARaddress(bus,slot,function,0x10);
                        unsigned long usbint = getBARaddress(bus,slot,function,0x3C) & 0x000000FF;
                        initialise_xhci_driver(bar0,usbint);
                    }
                    #endif
                }
            }
        }
    }
    // scan for available drivers
	for(int bus = 0 ; bus < 256 ; bus++){
		for(int slot = 0 ; slot < 32 ; slot++){
			for(int function = 0 ; function <= 7 ; function++){
				unsigned short vendor = pciConfigReadWord(bus,slot,function,0);
				unsigned short device = pciConfigReadWord(bus,slot,function,2);
				if(vendor != 0xFFFF){
					unsigned char classc = (pciConfigReadWord(bus,slot,function,0x0A)>>8)&0xFF;
					unsigned char sublca = (pciConfigReadWord(bus,slot,function,0x0A))&0xFF;
					unsigned char subsub = (pciConfigReadWord(bus,slot,function,0x08)>>8)&0xFF;
					k_printf("PCI-device: vendor:%x device:%x classc:%x sublca:%x subsub:%x \n",vendor,device,classc,sublca,subsub);
                    
                }
            }
        }
    }
}