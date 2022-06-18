#include "../include/xhci.h"
#include "../include/graphics.h"
#include "../include/memory.h"

unsigned long baseaddr;

XHCICapabilityRegisters* capability_registers;
XHCIOperationalRegisters* operational_registers;
XHCIRuntimeRegisters* runtime_registers;
unsigned long *dcbaap;
void *command_ring_control;
TransferEventTRB *event_ring_control;

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

uint8_t xhci_get_portcount(){
    return (capability_registers->hcsparams1>>24) & 0xFF;
}

void xhci_reset(){
    operational_registers->usbcmd |= 2;
    k_printf("xhci: reset controller\n");
    // reset
    while(operational_registers->usbcmd & 2);

    // set the portcount
    operational_registers->config |= xhci_get_portcount();

    event_ring_control = (TransferEventTRB*) requestPage();
    memset(event_ring_control,0,0x1000);
    XHCIEventRingSegmentTableEntry erse  __attribute__ ((aligned (0x100)));
    erse.address = (uint64_t) event_ring_control;
    erse.ringsegmentsize = 16;
    runtime_registers->intregs[0].erstsz |= 1;
    runtime_registers->intregs[0].erdp = (uint64_t) event_ring_control;
    runtime_registers->intregs[0].erstba = (uint64_t) &erse;

    // set crcr
    command_ring_control = requestPage();
    memset(command_ring_control,0,0x1000);
    operational_registers->crcr = ((uint64_t)command_ring_control) | 1;

    // set bcbaap
    dcbaap = (unsigned long*) requestPage();
    memset(dcbaap,0,0x1000);
    operational_registers->dcbaap = (uint64_t) dcbaap;

    // run!
    operational_registers->usbcmd |= 1;
}

void initialise_xhci_driver(unsigned long ba){
    baseaddr = ba & 0xFFFFFF00;
    k_printf("xhci: the xhci memory registers are stored at %x \n",baseaddr);
    capability_registers = (XHCICapabilityRegisters*) baseaddr;
    xhci_dump_capabilities();
    operational_registers = (XHCIOperationalRegisters*)(baseaddr + capability_registers->caplength);
    xhci_dump_operationals();
    runtime_registers = (XHCIRuntimeRegisters*)(baseaddr + capability_registers->rtsoff);
    xhci_reset();
    xhci_dump_operationals();
    while(1){
        if(event_ring_control[0].trb_type==XHCI_TRB_TYPE_EVENT_PORT_STATUS_CHANGE_EVENT){
            break;
        }
    }
    k_printf("We have a event here!\n");
    for(;;);
}