#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/pci.h"
#include "../include/idt.h"
#include "../include/memory.h"
#include "../include/timer.h"
#include "../include/usb.h"
#include "../include/ohci.h"

void *ohci_base_addr;

uint32_t getOHCIHcRevision()
{
    return ((uint32_t*)(ohci_base_addr+0x00))[0];
}

uint32_t getOHCIHcControl()
{
    return ((uint32_t*)(ohci_base_addr+0x04))[0];
}

uint32_t getOHCIHcCommandStatus()
{
    return ((uint32_t*)(ohci_base_addr+0x08))[0];
}

uint32_t getOHCIHcInterruptStatus()
{
    return ((uint32_t*)(ohci_base_addr+0x0C))[0];
}

uint32_t getOHCIHcInterruptEnable()
{
    return ((uint32_t*)(ohci_base_addr+0x10))[0];
}

uint32_t getOHCIHcInterruptDisable()
{
    return ((uint32_t*)(ohci_base_addr+0x14))[0];
}

void ohci_driver_start(int bus,int slot,int function)
{

    // print basic information about our session
    k_printf("ohci: found at bus %d slot %d and function %d !\n",bus,slot,function);
    ohci_base_addr = (void*) (getBARaddress(bus,slot,function,0x10) & 0xFFFFFFF0);
    k_printf("ohci: memoryarea: %x \n",ohci_base_addr);

    // we follow the 0x10 specification
    k_printf("ochi: this hostcontroller is compatible with the ohci specification of %x \n",getOHCIHcRevision());
    if(getOHCIHcRevision()!=0x10){
        k_printf("ochi: we do not support this revision: %x \n",getOHCIHcRevision());
        return;
    }

    // look at the status and see if something is different then we expect
    k_printf("ohci: hc controll value: %x commandstatus: %x interruptstatus: %x interruptenable: %x interruptdisable: %x \n",getOHCIHcControl(),getOHCIHcCommandStatus(),getOHCIHcInterruptStatus(),getOHCIHcInterruptEnable(),getOHCIHcInterruptDisable());

    // for(;;);
}