#pragma once 
#include "apci.h"
#define PCI_ADDRESS 0xCF8
#define PCI_DATA 0xCFC

typedef struct{

}PCIInfo;

void initialise_pci_driver();
void initialise_drivers_from_pci();