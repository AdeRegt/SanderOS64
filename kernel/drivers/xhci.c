#include "../include/xhci.h"
#include "../include/graphics.h"

unsigned long baseaddr;

XHCICapabilityRegisters* capability_registers;
XHCIOperationalRegisters* operational_registers;

void xhci_dump_capabilities(){
    k_printf("xhci: capabilities: caplength:%x hccparams1:%x hccparams2:%x hciversion:%x hcsparams1:%x hcsparams2:%x hcsparams3:%x rtsoff:%x \n",
        capability_registers->caplength,
        capability_registers->hccparams1,
        capability_registers->hccparams2,
        capability_registers->hciversion,
        capability_registers->hcsparams1,
        capability_registers->hcsparams2,
        capability_registers->hcsparams3,
        capability_registers->rtsoff
    );
}

void xhci_dump_operationals(){
    k_printf("xhci: operationals: config:%x crcr:%x dcbaap:%x dnctrl:%x pagesize:%x usbcmd:%x usbsts:%x \n",
        operational_registers->config,
        operational_registers->crcr,
        operational_registers->dcbaap,
        operational_registers->dnctrl,
        operational_registers->pagesize,
        operational_registers->usbcmd,
        operational_registers->usbsts
    );
}

void initialise_xhci_driver(unsigned long ba){
    baseaddr = ba & 0xFFFFFF00;
    k_printf("xhci: the xhci memory registers are stored at %x \n",baseaddr);
    capability_registers = (XHCICapabilityRegisters*) baseaddr;
    xhci_dump_capabilities();
    operational_registers = (XHCIOperationalRegisters*)(baseaddr + capability_registers->caplength);
    xhci_dump_operationals();
    for(;;);
}