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

void xhci_driver_start(int bus,int slot,int function);