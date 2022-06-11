#include "../include/pci.h"
#include "../include/kernel.h"
#include "../include/ports.h"

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

void initialise_pci_driver(){
    k_printf("Scanning for diskdriver to find bootdevice...\n");
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
                    if(classc==1&&sublca==6){
                        // this is a ahci driver! we can do this...
                        k_printf("ahci device found...\n");
                    }else{
                        // loadup external drivers
                        k_printf("no driver available for this device!\n");
                    }
                }
            }
        }
    }
}