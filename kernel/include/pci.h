#pragma once 
#include "apci.h"
#define PCI_ADDRESS 0xCF8
#define PCI_DATA 0xCFC

typedef struct{
    int bus;
    int slot;
    int function;
    uint32_t bar1;
    int inter;
}__attribute__((packed)) PCIInfo;

void initialise_pci_driver();
void initialise_drivers_from_pci();
unsigned long getBARaddress(int bus,int slot,int function,int barNO);
void setBARaddress(int bus,int slot,int function,int barNO,unsigned long result);
unsigned short pciConfigReadWord (unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset);
char pci_enable_busmastering_when_needed(int bus,int slot,int function);

void xhci_driver_start(int bus,int slot,int function);
void rtl_driver_start(int bus,int slot,int function);
void e1000_driver_start(int bus,int slot,int function);
void ehci_driver_start(int bus,int slot,int function);