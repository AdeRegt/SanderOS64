#include "../include/pci.h"
#include "../include/kernel.h"
#include "../include/ports.h"
#include "../include/ahci.h"
#include "../include/exec/module.h"
#include "../include/device.h"
#include "../include/ethernet.h"
#include "../include/ide.h"

unsigned long getPCIConfiguration(int bus,int slot,int function){
	return getBARaddress(bus,slot,function,0x04) & 0x0000FFFF;
}

char pci_enable_busmastering_when_needed(int bus,int slot,int function){
	if(!(getPCIConfiguration(bus,slot,function)&0x04)){
        k_printf("[PCI] PCI bussmaster not enabled!\n");
        unsigned long setting = getPCIConfiguration(bus,slot,function);
        setting |= 0x04;
        setBARaddress(bus,slot,function,4,setting);
        if(!(getPCIConfiguration(bus,slot,function)&0x04)){
            k_printf("[PCI] PCI busmastering still not enabled! Quiting...\n");
            return 0;
        }else{
            k_printf("[PCI] PCI busmastering is now enabled.\n");
        }
    }
	return 1;
}

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

void pciConfigWriteWord (unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset,unsigned long value) {
    unsigned long address;
    unsigned long lbus  = (unsigned long)bus;
    unsigned long lslot = (unsigned long)slot;
    unsigned long lfunc = (unsigned long)func;
 
    /* create configuration address as per Figure 1 */
    address = (unsigned long)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((unsigned long)0x80000000));
 
    /* write out the address */
    outportl(PCI_ADDRESS, address);
    outportl(PCI_DATA,value);
}

unsigned long getBARaddress(int bus,int slot,int function,int barNO){
	unsigned long result = 0;
	unsigned long partA = pciConfigReadWord(bus,slot,function,barNO);
	unsigned long partB = pciConfigReadWord(bus,slot,function,barNO+2);
	result = ((partB<<16) | ((partA) & 0xffff));
	return result;
}

void setBARaddress(int bus,int slot,int function,int barNO,unsigned long result){
	unsigned long partA = result & 0xffff;
	unsigned long partB = (result>>16) & 0xffff;
	pciConfigWriteWord(bus,slot,function,barNO+0,partA);
	pciConfigWriteWord(bus,slot,function,barNO+2,partB);
}

void initialise_drivers_from_pci(){
    // scan for available drivers
    char* filedir = dir("A:SANDEROS/DRIVERS");
    if(filedir){
        k_printf("Available drivers: %s \n",filedir);
    }
	for(int bus = 0 ; bus < 256 ; bus++){
		for(int slot = 0 ; slot < 32 ; slot++){
			for(int function = 0 ; function <= 7 ; function++){
				unsigned short vendor = pciConfigReadWord(bus,slot,function,0);
				unsigned short device = pciConfigReadWord(bus,slot,function,2);
				if(vendor != 0xFFFF){
					unsigned char classc = (pciConfigReadWord(bus,slot,function,0x0A)>>8)&0xFF;
					unsigned char sublca = (pciConfigReadWord(bus,slot,function,0x0A))&0xFF;
					unsigned char subsub = (pciConfigReadWord(bus,slot,function,0x08)>>8)&0xFF;
                    PCIInfo pi;
                    pi.bus = bus;
                    pi.slot = slot;
                    pi.function = function;
                    pi.bar1 = getBARaddress(bus,slot,function,0x10) ;
                    pi.inter = getBARaddress(bus,slot,function,0x3C) & 0x000000FF;
					if( classc==0x0C && sublca==0x03 && subsub==0x30 ){
                        // loadModule("A:SANDEROS/DRIVERS/XHCI.SYS",(PCIInfo*)&pi);
                    }else if( classc==0x02 && sublca==0x00 && (device==0x8168||device==0x8139)&&vendor==0x10ec ){
                        #ifdef use_driver
                        loadModule("A:SANDEROS/DRIVERS/RTL.SYS",(PCIInfo*)&pi);
                        #elif enable_ethernet
                        rtl_driver_start(bus,slot,function);
                        #endif
                    }else if( classc==0x02 && sublca==0x00 && (device==0x100e||device==0x153A||device==0x10EA||vendor==0x8086)){
                        #ifdef enable_ethernet
                        e1000_driver_start(bus,slot,function);
                        #endif
                    }
                }
            }
        }
    }
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
					unsigned char subsub = (pciConfigReadWord(bus,slot,function,0x08)>>8)&0xFF;
                    if(classc==0x01&&sublca==0x06){
                        unsigned long bar5 = getBARaddress(bus,slot,function,0x24);
                        unsigned long usbint = getBARaddress(bus,slot,function,0x3C) & 0x000000FF;
                        initialise_ahci_driver(bar5,usbint);
                        continue;
                    }else if( classc==0x0C && sublca==0x03 && subsub==0x30 ){
                        xhci_driver_start(bus,slot,function);
                    }else if(classc==0x0C && sublca==0x03 && subsub==0x20 ){
                        ehci_driver_start(bus,slot,function);
                    }else if(classc==0x01 && sublca==0x01 ){
                        ide_driver_start(bus,slot,function);
                    }
                }
            }
        }
    }
}