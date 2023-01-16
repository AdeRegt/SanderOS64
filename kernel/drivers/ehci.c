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

#define USB2_DESCRIPTOR_TYPE_POWER  8
#define USB2_DESCRIPTOR_TYPE_OTHER  7
#define USB2_DESCRIPTOR_TYPE_QUALI  6
#define USB2_DESCRIPTOR_TYPE_ENDPOINT 5
#define USB2_DESCRIPTOR_TYPE_INTERFACE 4
#define USB2_DESCRIPTOR_TYPE_STRING 3
#define USB2_DESCRIPTOR_TYPE_CONFIGURATION 2
#define USB2_DESCRIPTOR_TYPE_DEVICE 1

#define USB2_REQUEST_SYNCH_FRAME 12
#define USB2_REQUEST_SET_INTERFACE 11
#define USB2_REQUEST_GET_INTERFACE 10
#define USB2_REQUEST_SET_CONFIGURATION 9
#define USB2_REQUEST_GET_CONFIGURATION 8
#define USB2_REQUEST_SET_DESCRIPTOR 7
#define USB2_REQUEST_GET_DESCRIPTOR 6
#define USB2_REQUEST_SET_ADDRESS 5
#define USB2_REQUEST_SET_FEATURE 3
#define USB2_REQUEST_CLEAR_FEATURE 1
#define USB2_REQUEST_GET_STATUS 0

typedef struct  {
    uint8_t bRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} EhciCMD;

typedef struct {
    volatile uint32_t nextlink;
    volatile uint32_t altlink;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t extbuffer[5];
} EhciTD;

typedef struct {
    volatile uint32_t horizontal_link_pointer;
    volatile uint32_t characteristics;
    volatile uint32_t capabilities;
    volatile uint32_t curlink;

    volatile uint32_t nextlink;
    volatile uint32_t altlink;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t extbuffer[5];
}EhciQH;

typedef struct __attribute__ ((packed)){
    unsigned char  bLength;
    unsigned char  bDescriptorType;

    unsigned short wTotalLength;
    unsigned char  bNumInterfaces;
    unsigned char  bConfigurationValue;
    unsigned char  iConfiguration;
    unsigned char  bmAttributes;
    unsigned char  bMaxPower;
}usb_config_descriptor ;

typedef struct __attribute__ ((packed)) {
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  bInterfaceProtocol;
    uint8_t  iInterface;
}usb_interface_descriptor;

typedef struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bEndpointAddress;
	unsigned char bmAttributes;
	unsigned short wMaxPacketSize;
	unsigned char bInterval;
}EHCI_DEVICE_ENDPOINT;

void *ehci_base_addr;
uint8_t caplength;
unsigned long periodic_list[EHCI_PERIODIC_FRAME_SIZE] __attribute__ ((aligned (0x1000)));
uint8_t ehci_available_ports;
uint8_t ehci_address_count = 1;

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
        ((uint32_t*)(ehci_base_addr + caplength + 4))[0] |= 0x10;
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

EhciCMD *ehci_generate_command_structure(uint8_t request, uint8_t dir, uint8_t type, uint8_t recieve, uint16_t windex,uint16_t wlength, uint16_t wvalue)
{
    EhciCMD *command = (EhciCMD*) requestPage();
    memset(command,0,sizeof(EhciCMD));
    command->bRequest = request; // set address
    command->bRequestType = 0;
    command->bRequestType |= dir; // dir is out
    command->bRequestType |= (type<<5); // type is standard
    command->bRequestType |= recieve; // recieve 
    command->wIndex = windex; 
    command->wLength = wlength;
    command->wValue = wvalue;
    return command;
}

EhciTD *ehci_generate_transfer_descriptor(uint32_t nextlink,uint8_t type,uint8_t size,uint8_t toggle,uint32_t data)
{
    EhciTD *command = (EhciTD*) requestPage();
    memset(command,0,sizeof(EhciTD));
    command->nextlink = nextlink;
    command->altlink = 1;
    command->token = 0;
    command->token |= (1<<7); // actief
    command->token |= (type<<8); 
    command->token |= (3<<10); // maxerror
    command->token |= (size<<16);
    command->token |= (toggle<<31);
    command->buffer[0] = data;
    return command;
}

EhciQH *ehci_generate_queue_head(uint32_t next_link,uint8_t eps,uint8_t dtc,uint8_t t,uint8_t mplen,uint8_t device,uint32_t capabilities,uint32_t token)
{
    EhciQH *command = (EhciQH*) requestPage();
    memset(command,0,sizeof(EhciQH));
    command->altlink = 1;
    command->nextlink = next_link;
    command->curlink = 0;
    command->characteristics = 0;
    command->characteristics |= (eps<<12);
    command->characteristics |= (dtc<<14);
    command->characteristics |= (t<<15);
    command->characteristics |= (mplen<<16);
    command->characteristics |= (device);
    command->capabilities = capabilities;
    command->token = token;
    return command;
}

uint8_t ehci_wait_for_completion(volatile EhciTD *status)
{
    uint8_t lstatus = 1;
    k_printf("ehci: waiting for completion qh: ");
    int timeout = 25;
    while(1)
    {
        sleep(10);
        k_printf("*");
        volatile uint32_t tstatus = (volatile uint32_t)status->token;
        if(tstatus & (1 << 4))
        {
            // not anymore active and failed miserably
            k_printf("[EHCI] Transmission failed due babble error\n");
            lstatus = 0;
            break;
        }
        if(tstatus & (1 << 3))
        {
            // not anymore active and failed miserably
            k_printf("[EHCI] Transmission failed due transaction error\n");
            lstatus = 0;
            break;
        }
        if(tstatus & (1 << 6))
        {
            // not anymore active and failed miserably
            k_printf("[EHCI] Transmission failed due serious error\n");
            lstatus = 0;
            break;
        }
        if(tstatus & (1 << 5))
        {
            // not anymore active and failed miserably
            k_printf("[EHCI] Transmission failed due data buffer error\n");
            lstatus = 0;
            break;
        }
        if(!(tstatus & (1 << 7)))
        {
            // not anymore active and succesfull ended
            // k_printf("[EHCI] Transaction succeed\n");
            k_printf("\n");
            lstatus = 1;
            break;
        }
        timeout--;
        if(timeout==0)
        {
            k_printf("[EHCI] Timeout\n");
            lstatus = 0;
            break;
        }
    }
    return lstatus;
}

uint8_t ehci_offer_queuehead_to_ring(uint32_t qh,EhciTD *stat)
{
    ((uint32_t*)(ehci_base_addr + caplength + 0x18 ))[0] = (uint32_t) qh;
    ((uint32_t*)(ehci_base_addr + caplength))[0] |= 0b100000;
    uint8_t res = ehci_wait_for_completion(stat);
    ((uint32_t*)(ehci_base_addr + caplength))[0] &= ~0b100000;
    ((uint32_t*)(ehci_base_addr + caplength + 0x18 ))[0] = 1;
    return res;
}

uint8_t ehci_request_device_addr(uint8_t wantedaddress)
{
    EhciCMD *command = ehci_generate_command_structure(USB2_REQUEST_SET_ADDRESS,0,0,0,0,0,wantedaddress);
    EhciTD *status = ehci_generate_transfer_descriptor(1,1,0,1,0);
    EhciTD *transfercommand = ehci_generate_transfer_descriptor((uint32_t)(upointer_t)status,2,8,0,(uint32_t)(upointer_t)command);
    EhciQH *head1 = ehci_generate_queue_head(1,0,0,1,0,0,0,0x40);
    EhciQH *head2 = ehci_generate_queue_head((uint32_t)(upointer_t)transfercommand,2,1,0,64,0,0x40000000,0);
    head1->horizontal_link_pointer = ((uint32_t)(upointer_t)head2) | 2;
    head2->horizontal_link_pointer = ((uint32_t)(upointer_t)head1) | 2;
    uint8_t res = ehci_offer_queuehead_to_ring((uint32_t)(upointer_t)head1,status);
    freePage(command);
    freePage(status);
    freePage(transfercommand);
    freePage(head1);
    freePage(head2);
    return res;
}

void *ehci_request_device_descriptor(uint8_t address,uint8_t type,uint8_t index,uint8_t size)
{
    void *buffer2 = requestPage();
    memset(buffer2,0,sizeof(EhciQH));
    void *buffer = requestPage();
    EhciCMD *command = ehci_generate_command_structure(USB2_REQUEST_GET_DESCRIPTOR,0,4,0,0,size,(type << 8) | index);                                                          // OK
    EhciTD *status = ehci_generate_transfer_descriptor(1,0,0,1,0);                                                                      // OK
    EhciTD *transfercommand = ehci_generate_transfer_descriptor((uint32_t)(upointer_t)status,1,size,1,(uint32_t)(upointer_t)buffer);    // OK
    EhciTD *td = ehci_generate_transfer_descriptor((uint32_t)(upointer_t)transfercommand,2,8,0,(uint32_t)(upointer_t)command);       // OK 8
    EhciQH *head1 = ehci_generate_queue_head(1,0,0,1,0,0,0,0x40);
    EhciQH *head2 = ehci_generate_queue_head((uint32_t)(upointer_t)td,2,1,0,64,address,0x40000000,0);
    head1->horizontal_link_pointer = ((uint32_t)(upointer_t)head2) | 2;
    head2->horizontal_link_pointer = ((uint32_t)(upointer_t)head1) | 2;
    head2->curlink = (uint32_t) (upointer_t) buffer2;
    uint8_t res = ehci_offer_queuehead_to_ring((uint32_t)(upointer_t)head1,status);
    freePage(command);
    freePage(status);
    freePage(transfercommand);
    freePage(td);
    freePage(head1);
    freePage(head2);
    if(res){
        return buffer;
    }else{
        return 0;
    }
}

void ehci_test_port(int portno)
{
    volatile uint32_t portreg = ((volatile uint32_t*)(ehci_base_addr + caplength + 0x44 + ( 4 * ( portno  - 1 ) ) ))[0];
    if(!(portreg&3))
    {
        // no connection, halt
        return;
    }
    k_printf("ehci-%d: found connection width status %x \n",portno,portreg);

    uint8_t device_address = ehci_address_count++;

    //
    // our new device needs an address...
    uint8_t rdar = ehci_request_device_addr(device_address);
    if(rdar==0)
    {
        goto failed;
    }
    k_printf("ehci-%d: device-address: %d \n",portno,device_address);

    //
    // we need some information about our device
    void *devicedescriptor = ehci_request_device_descriptor(device_address,USB2_DESCRIPTOR_TYPE_DEVICE,0,8);
    if(devicedescriptor==0)
    {
        goto failed;
    }
    freePage(devicedescriptor);

    devicedescriptor = ehci_request_device_descriptor(device_address,USB2_DESCRIPTOR_TYPE_CONFIGURATION,0,sizeof(usb_interface_descriptor) + sizeof(usb_config_descriptor)+(sizeof(EHCI_DEVICE_ENDPOINT)*2));
    if(devicedescriptor==0)
    {
        goto failed;
    }
    usb_interface_descriptor* desc = (usb_interface_descriptor*)(((unsigned long)devicedescriptor)+sizeof(usb_config_descriptor));
    k_printf("ehci-%d: There are %d endpoints available!\n",portno,desc->bNumEndpoints);
    EHCI_DEVICE_ENDPOINT *ep1 = (EHCI_DEVICE_ENDPOINT*)(((unsigned long)devicedescriptor)+sizeof(usb_config_descriptor)+sizeof(usb_interface_descriptor));
    EHCI_DEVICE_ENDPOINT *ep2 = (EHCI_DEVICE_ENDPOINT*)(((unsigned long)devicedescriptor)+sizeof(usb_config_descriptor)+sizeof(usb_interface_descriptor)+7);
    k_printf("ehci-%d: EP1 size=%x type=%x dir=%c num=%x epsize=%x \n",portno,ep1->bLength,ep1->bDescriptorType,ep1->bEndpointAddress&0x80?'I':'O',ep1->bEndpointAddress&0xF,ep1->wMaxPacketSize&0x7FF);
    k_printf("ehci-%d: EP2 size=%x type=%x dir=%c num=%x epsize=%x \n",portno,ep2->bLength,ep2->bDescriptorType,ep2->bEndpointAddress&0x80?'I':'O',ep2->bEndpointAddress&0xF,ep2->wMaxPacketSize&0x7FF);
    k_printf("ehci-%d: class=%x subclass=%x \n",portno,desc->bInterfaceClass,desc->bInterfaceSubClass);
    
    freePage(devicedescriptor);

    for(;;);
    return;
    failed:
    k_printf("ehci-%d: sadly we failed to configure this device!\n",portno);
    for(;;);
    return;
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
    ((uint32_t*)(ehci_base_addr + caplength))[0] = 0x80001 | (0x40<<16);//0x80011 | (0x40<<16);

    //
    // wait untill everything should be ready
    sleep(25);

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