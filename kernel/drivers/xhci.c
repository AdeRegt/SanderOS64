#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/pci.h"
#include "../include/idt.h"
#include "../include/memory.h"
#include "../include/timer.h"
#include "../include/usb.h"

typedef struct{
    uint32_t ring_segment_base_address_low;
    uint32_t ring_segment_base_address_high;
    uint16_t ring_segment_size;
    uint16_t reservedA;
    uint32_t reservedB;
}__attribute__((packed)) XHCIEventRingSegmentTable;

void *xhci_base_addr;
uint8_t xhci_capability_registers_length = 0;
uint8_t xhci_number_of_ports = 0;
uint32_t xhci_doorbell_offset = 0;
uint32_t xhci_runtime_offset = 0;

void *dcbaap;
void *commandring;
void *eventring;
XHCIEventRingSegmentTable *segmenttable;

uint32_t xhci_get_usbcmd_reg()
{
    return ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0))[0];
}

void xhci_set_usbcmd_reg(uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0))[0] = nv;
}

uint32_t xhci_get_usbsts_reg()
{
    return ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x04))[0];
}

void xhci_set_usbsts_reg(uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x04))[0] = nv;
}

uint32_t xhci_get_pagesize_reg()
{
    return ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x08))[0];
}

void xhci_set_pagesize_reg(uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x08))[0] = nv;
}

uint32_t xhci_get_dnctrl_reg()
{
    return ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x14))[0];
}

void xhci_set_dnctrl_reg(uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x14))[0] = nv;
}

uint32_t xhci_get_crcr_reg()
{
    return ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x18))[0];
}

void xhci_set_crcr_reg(uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x18))[0] = nv;
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x18+4))[0] = 0;
}

uint32_t xhci_get_dcbaap_reg()
{
    return ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x30))[0];
}

void xhci_set_dcbaap_reg(uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x30))[0] = nv;
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x30+4))[0] = 0;
}

uint32_t xhci_get_config_reg()
{
    return ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x38))[0];
}

void xhci_set_config_reg(uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+0x38))[0] = nv;
}

uint32_t xhci_get_portsc_reg(uint8_t n)
{
    return ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+(0x400 + (0x10 * (n-1)))))[0];
}

void xhci_set_portsc_reg(uint8_t n, uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_capability_registers_length+(0x400 + (0x10 * (n-1)))))[0] = nv;
}

uint32_t xhci_get_erstsz_reg(uint8_t n)
{
    return ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x28 + (32 * n))))[0];
}

void xhci_set_erstsz_reg(uint8_t n, uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x28 + (32 * n))))[0] = nv;
}

uint32_t xhci_get_erdp_reg(uint8_t n)
{
    return ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x38 + (32 * n))))[0];
}

void xhci_set_erdp_reg(uint8_t n, uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x38 + (32 * n))))[0] = nv;
    ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x38 + 4 + (32 * n))))[0] = 0;
}

uint32_t xhci_get_erstba_reg(uint8_t n)
{
    return ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x30 + (32 * n))))[0];
}

void xhci_set_erstba_reg(uint8_t n, uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x30 + (32 * n))))[0] = nv;
    ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x30 + 4 + (32 * n))))[0] = 0;
}

uint32_t xhci_get_imod_reg(uint8_t n)
{
    return ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x24 + (32 * n))))[0];
}

void xhci_set_imod_reg(uint8_t n, uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x24 + (32 * n))))[0] = nv;
}

uint32_t xhci_get_iman_reg(uint8_t n)
{
    return ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x20 + (32 * n))))[0];
}

void xhci_set_iman_reg(uint8_t n, uint32_t nv)
{
    ((uint32_t*)(xhci_base_addr+xhci_runtime_offset+(0x20 + (32 * n))))[0] = nv;
}

__attribute__((interrupt)) void irq_xhci(interrupt_frame* frame)
{
    asm volatile ("cli");
    uint32_t usbsts = xhci_get_usbsts_reg();
    uint32_t usbiman = xhci_get_iman_reg(0);
    k_printf("xhci: interrupt found: usbsts:%x iman:%x \n",usbsts,usbiman);
    if(usbsts&0b1000)
    {
        k_printf("xhci: event interrupt!\n");
    }
    if(usbsts&0b10000)
    {
        k_printf("xhci: portchange interrupt!\n");
    }
    xhci_set_usbsts_reg(usbsts);
    xhci_set_erdp_reg(0,((uint32_t)(upointer_t)eventring) & 8);
    xhci_set_iman_reg(0,usbiman);
    interrupt_eoi();
    asm volatile ("sti");
}

void xhci_port_install(uint8_t portid)
{
    uint32_t portc = xhci_get_portsc_reg(portid);
    if((portc&3)!=3)
    {
        return;
    }
    k_printf("xhci: portid:%d porthex:%x ",portid,portc);
}

void xhci_driver_start(int bus,int slot,int function)
{
    k_printf("xhci: Driver started at bus %d slot %d and function %d \n",bus,slot,function);

    //
    // Enable busmastering
    pci_enable_busmastering_when_needed(bus,slot,function);

    xhci_base_addr = (void*) (getBARaddress(bus,slot,function,0x10) & 0xFFFFFFF0);

    unsigned long usbint = getBARaddress(bus,slot,function,0x3C) & 0x000000FF;
	setInterrupt(usbint,irq_xhci);

    unsigned long hcversion = getBARaddress(bus,slot,function,0x60) & 0x000000FF;
    if(hcversion!=0x30)
    {
        k_printf("xhci: invalid xhci version (%x)!\n",hcversion);
        return;
    }

    xhci_capability_registers_length = ((uint8_t*)xhci_base_addr)[0];

    uint16_t hcinterfaceversion = ((uint16_t*)(xhci_base_addr+2))[0];
    if(hcinterfaceversion!=0x100)
    {
        k_printf("xhci: invalid xhci interface version: %x \n",hcinterfaceversion);
        return;
    }

    xhci_number_of_ports = ((uint32_t*)(xhci_base_addr+0x04))[0]>>24;
    k_printf("xhci: this computer has %d xhci ports available!\n",xhci_number_of_ports);

    xhci_doorbell_offset = ((uint32_t*)(xhci_base_addr+0x14))[0];

    xhci_runtime_offset = ((uint32_t*)(xhci_base_addr+0x18))[0];

    //
    // check the capabilities to stop the system
    uint32_t capapointer = (((uint32_t*)(xhci_base_addr+0x10))[0]>>16)<<2;
    void *cappointer = capapointer + xhci_base_addr;
    while(1)
    {
        uint32_t reg = ((uint32_t*)cappointer)[0];
        uint8_t capid = reg & 0xFF;
        uint8_t capoffset = (reg>>8) & 0xFF;
        if(capid==0)
        {
            break;
        }
        if( capid==1 && reg & 0x10000 )
        {
            k_printf("xhci: found legacy problems\n");
            ((uint32_t*)cappointer)[0] |= 0x1000000;
            sleep(1);
            continue;
        }
        if(capoffset==0)
        {
            break;
        }
        cappointer += capoffset * sizeof(uint32_t);
    }

    //
    // start resetsequence...
    // we need first to stop any running processes...
    onceagainstoprunning:
    if(xhci_get_usbcmd_reg()&1)
    {
        // we are still running...
        // we need to stop running!
        uint32_t ov = xhci_get_usbcmd_reg();
        ov &= ~1; // unset run bit
        xhci_set_usbcmd_reg(ov);
        sleep(1);
        goto onceagainstoprunning;
    }
    // we want to make 100% sure it is not running by checking the halt bit
    if((xhci_get_usbsts_reg()&1)==0)
    {
        goto onceagainstoprunning;
    }
    // at this point it is safe to assume the controller is stopped.
    // lets trigger a reset.
    // send reset sequence
    xhci_set_usbcmd_reg(2);
    // wait untill reset is completed
    while(xhci_get_usbcmd_reg()&2)
    {
        sleep(1);
    }
    // wait untill controller is ready
    while(xhci_get_usbsts_reg()&(1<<11))
    {
        sleep(1);
    }
    // we survived the reset!
    // now check if everything is in the state we expect it to be!
    if(xhci_get_usbcmd_reg()!=0)
    {
        k_printf("xhci: USBCMD unexpected state after reset!\n");
        return;
    }
    if(!(xhci_get_usbsts_reg()==1||xhci_get_usbsts_reg()==0x11))
    {
        k_printf("xhci: USBSTS unexpected state after reset: %x !\n",xhci_get_usbsts_reg());
        return;
    }
    if(xhci_get_dnctrl_reg()!=0)
    {
        k_printf("xhci: dnctrl unexpected state after reset!\n");
        return;
    }
    if(xhci_get_crcr_reg()!=0)
    {
        k_printf("xhci: crcr unexpected state after reset!\n");
        return;
    }
    if(xhci_get_dcbaap_reg()!=0)
    {
        k_printf("xhci: dcbaap unexpected state after reset!\n");
        return;
    }
    if(xhci_get_config_reg()!=0)
    {
        k_printf("xhci: config unexpected state after reset!\n");
        return;
    }

    // Program the Max Device Slots Enabled (MaxSlotsEn) field in the CONFIG
    // register (5.4.7) to enable the device slots that system software is going to use.
    xhci_set_config_reg(xhci_get_config_reg() | xhci_number_of_ports);

    // Program the Device Context Base Address Array Pointer (DCBAAP)
    // register (5.4.6) with a 64-bit address pointing to where the Device
    // Context Base Address Array is located.
    dcbaap = requestPage();
    memset(dcbaap,0,0x1000);
    xhci_set_dcbaap_reg((uint32_t)(upointer_t)dcbaap);

    // if there are scratchpads required create them here!
    uint32_t* scratchpad = (uint32_t*) requestPage();
    memset(scratchpad,0,0x1000);
    for(int i = 0 ; i < 2 ; i++){
        scratchpad[i*2] = (uint32_t)(upointer_t) requestPage();
    }
    ((uint32_t*)dcbaap)[0] = (uint32_t)(upointer_t)scratchpad;
    ((uint32_t*)dcbaap)[1] = (uint32_t)(upointer_t)0;

    // Define the Command Ring Dequeue Pointer by programming the
    // Command Ring Control Register (5.4.5) with a 64-bit address pointing to
    // the starting address of the first TRB of the Command Ring.
    commandring = requestPage();
    memset(commandring,0,0x1000);
    xhci_set_crcr_reg((uint32_t)(upointer_t)commandring);

    // Allocate and initialize the Event Ring Segment(s).
    eventring = requestPage();
    memset(eventring,0,0x1000);

    // Allocate the Event Ring Segment Table (ERST)
    // (section 6.5). Initialize ERST table entries to point
    // to and to define the size (in TRBs) of the respective
    // Event Ring Segment.
    segmenttable = (XHCIEventRingSegmentTable*) requestPage();
    memset(segmenttable,0,sizeof(XHCIEventRingSegmentTable));
    segmenttable->ring_segment_base_address_low = (uint32_t)(upointer_t)eventring;
    segmenttable->ring_segment_size = 16;

    // Program the Interrupter Event Ring Segment Table
    // Size (ERSTSZ) register (5.5.2.3.1) with the number
    // of segments described by the Event Ring Segment
    // Table.
    xhci_set_erstsz_reg(0,1);

    // Program the Interrupter Event Ring Dequeue
    // Pointer (ERDP) register (5.5.2.3.3) with the starting
    // address of the first segment described by the
    // Event Ring Segment Table.
    xhci_set_erdp_reg(0,(uint32_t)(upointer_t)eventring);

    // Program the Interrupter Event Ring Segment Table
    // Base Address (ERSTBA) register (5.5.2.3.2) with a
    // 64-bit address pointer to where the Event Ring
    // Segment Table is located.
    xhci_set_erstba_reg(0,(uint32_t)(upointer_t)segmenttable);

    // Initializing the Interval field of the Interrupt
    // Moderation register (5.5.2.2) with the target
    // interrupt moderation rate.
    xhci_set_imod_reg(0,0x00100FA0);

    // Enable system bus interrupt generation by writing
    // a ‘1’ to the Interrupter Enable (INTE) flag of the
    // USBCMD register (5.4.1).
    xhci_set_usbcmd_reg(4);

    // Enable the Interrupter by writing a ‘1’ to the
    // Interrupt Enable (IE) field of the Interrupter
    // Management register (5.5.2.1).
    xhci_set_iman_reg(0,2);

    // Write the USBCMD (5.4.1) to turn the host controller ON via setting the
    // Run/Stop (R/S) bit to ‘1’. This operation allows the xHC to begin
    // accepting doorbell references.
    xhci_set_usbcmd_reg(5);

    k_printf("xhci: OK\n");
    sleep(5);
    for(uint8_t i = 1 ; i < xhci_number_of_ports ; i++){
        xhci_port_install(i);
    }
    for(;;);
}