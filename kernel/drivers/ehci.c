#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/pci.h"
#include "../include/idt.h"
#include "../include/memory.h"
#include "../include/timer.h"

#define EHCI_PCI_INFO_REG           0x60
#define EHCI_PROTOCOL_VER           0x20
#define EHCI_HCI_VER                0x100
#define EHCI_PERIODIC_FRAME_SIZE    1024

void *ehci_base_addr;
uint8_t caplength;
unsigned long periodic_list[EHCI_PERIODIC_FRAME_SIZE] __attribute__ ((aligned (0x1000)));
uint8_t ehci_available_ports;

uint32_t ehci_get_usbcmd()
{
    return ((uint32_t*)(ehci_base_addr + caplength + 0))[0];
}

uint32_t ehci_get_usbsts()
{
    return ((uint32_t*)(ehci_base_addr + caplength + 4))[0];
}

__attribute__((interrupt)) void irq_ehci(interrupt_frame* frame)
{
    uint32_t status = ehci_get_usbsts();
    if(status&0x20)
    {
        k_printf("ehci: Interrupt on Async Advance!\n");
    }
    if(status&0x10)
    {
        k_printf("ehci: host system error!\n");
    }
    if(status&0x8)
    {
        ((uint32_t*)(ehci_base_addr + caplength + 4))[0] |= 8;
    }
    if(status&0x4)
    {
        k_printf("ehci: portchange!\n");
    }
    if(status&0x2)
    {
        k_printf("ehci: error!\n");
    }
    if(status&0x1)
    {
        k_printf("ehci: trb completion!\n");
    }
    interrupt_eoi();
}

void ehci_test_port(int portno)
{
    volatile uint32_t portreg = ((volatile uint32_t*)(ehci_base_addr + caplength + 0x44 + ( 4 * ( portno  - 1 ) ) ))[0];
    if(!(portreg&3))
    {
        // no connection, halt
        return;
    }
    k_printf("YAY!!!");
}

void ehci_probe_ports()
{
    for(int i = 1 ; i < ehci_available_ports ; i++)
    {
        ehci_test_port(i);
    }
}

void ehci_driver_start(int bus,int slot,int function)
{

    //
    // Enable busmastering
    pci_enable_busmastering_when_needed(bus,slot,function);

    //
    // get BAR address first
    ehci_base_addr = (void*) (getBARaddress(bus,slot,function,0x10) & 0xFFFFFFFE);

    // 
    // do check the version of the protocol
    uint16_t ehci_info = pciConfigReadWord(bus,slot,function,EHCI_PCI_INFO_REG);
    uint8_t ehci_version = ehci_info & 0xFF;
    if(ehci_version!=EHCI_PROTOCOL_VER)
    {
        k_printf("ehci: invalid version number\n");
        return;
    }

    // 
    // do check the version of the hci 
    uint16_t hci_ver = ((uint16_t*)(ehci_base_addr+2))[0];
    if(hci_ver!=EHCI_HCI_VER)
    {
        k_printf("ehci: invalid hci version number\n");
        return;
    }

    //
    // get the capability register
    caplength = ((uint8_t*)(ehci_base_addr))[0];

    //
    // register the interrupt handler 
    unsigned long usbint = getBARaddress(bus,slot,function,0x3C) & 0x000000FF;
	setInterrupt(usbint,irq_ehci);

    //
    // get amount of ports
    ehci_available_ports = ((uint32_t*)(ehci_base_addr + 4))[0] & 0b1111;

    //
    // have a fight with the legacy people
    uint8_t capreg = (((uint32_t*)(ehci_base_addr + 8))[0] & 0b1111111100000000) >> 8;
    if(capreg)
    {
        while(1)
        {
            uint32_t capid = getBARaddress(bus,slot,function,capreg);
            if((capid&0xFF)==1)
            {

                //
                // we need to own the controller!
                if(capid&0x10000)
                {
                    capid &= ~0x10000;
                    capid |= 0x1000000;
                    setBARaddress(bus,slot,function,capreg,capid);
                    sleep(10);
                    continue;
                }
            }
            uint16_t posnext = (capid >> 8) & 0xFF;
            if(posnext)
            {
                capreg += posnext;
            }
            else
            {
                break;
            }
        }
    }

    //
    // Are we running? 
    // if yes, stop us!
    if(ehci_get_usbcmd()&1)
    {
        ((uint32_t*)(ehci_base_addr + caplength))[0] &= ~1; // unset bit
        sleep(1); // wait a bit
        while(ehci_get_usbcmd()&1)
        {
            sleep(1);
        }
    }
    sleep(1);

    //
    // Reset the controller
    ((uint32_t*)(ehci_base_addr + caplength))[0] |= 2;
    sleep(1);
    while(ehci_get_usbcmd()&2)
    {
        sleep(1);
    }

    //
    // reset completed! check if all the default values are set!
    if(((uint32_t*)(ehci_base_addr + caplength + 0x00))[0]!=0x80000)
    {
        k_printf("ehci: warning: unexpected value after reset for USBCMD register!\n");
    }
    if(((uint32_t*)(ehci_base_addr + caplength + 0x04))[0]!=0x1000)
    {
        k_printf("ehci: warning: unexpected value after reset for USBSTS register!\n");
    }
    if(((uint32_t*)(ehci_base_addr + caplength + 0x08))[0]!=0)
    {
        k_printf("ehci: warning: unexpected value after reset for USBINT register!\n");
    }
    if(((uint32_t*)(ehci_base_addr + caplength + 0x0C))[0]!=0)
    {
        k_printf("ehci: warning: unexpected value after reset for FRINDEX register!\n");
    }
    if(((uint32_t*)(ehci_base_addr + caplength + 0x10))[0]!=0)
    {
        k_printf("ehci: warning: unexpected value after reset for CTRLDSSEGMENT register!\n");
    }
    if(((uint32_t*)(ehci_base_addr + caplength + 0x40))[0]!=0)
    {
        k_printf("ehci: warning: unexpected value after reset for CONFIGFLAG register!\n");
    }
    for(int i = 1 ; i < ehci_available_ports ; i++)
    {
        uint32_t pi = ((volatile uint32_t*)(ehci_base_addr + caplength + 0x44 + ( 4 * (i  - 1) ) ))[0];
        if(pi!=0x3000)
        {
            k_printf("ehci: warning: unexpected value after reset for PORTSC register: %x !\n",pi);
        }
    }

    //
    // clear periodic list
    for(int i = 0 ; i < EHCI_PERIODIC_FRAME_SIZE ; i++)
    {
        periodic_list[i] |= 1;
    }

    //
    // set segment to 0 (default)
    ((uint32_t*)(ehci_base_addr + caplength + 0x10 ))[0] = 0;

    //
    // enable all USB interrupts
    ((uint32_t*)(ehci_base_addr + caplength + 0x08 ))[0] = 0b111111;

    //
    // set the periodic list base
    ((uint32_t*)(ehci_base_addr + caplength + 0x14 ))[0] = (uint32_t) ((upointer_t)&periodic_list);

    //
    // run and enable the cycle
    ((uint32_t*)(ehci_base_addr + caplength))[0] = 0x80011 | (0x40<<16);

    //
    // wait untill everything should be ready
    sleep(50);

    //
    // We need to own everything!
    ((uint32_t*)(ehci_base_addr + caplength + 0x40 ))[0] |= 1;

    //
    // proof we do not hang!
    while(1)
    {
        sleep(10);
        k_printf("#");
        ehci_probe_ports();
    }
    for(;;);
}