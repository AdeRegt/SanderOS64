#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/pci.h"
#include "../include/idt.h"
#include "../include/memory.h"
#include "../include/timer.h"
#include "../include/usb.h"
#include "../include/xhci.h"

void *xhci_base_addr;
uint8_t xhci_capability_registers_length = 0;
uint8_t xhci_number_of_ports = 0;
uint32_t xhci_doorbell_offset = 0;
uint32_t xhci_runtime_offset = 0;

void *dcbaap;
void *commandring;
void *eventring;
XHCIEventRingSegmentTable *segmenttable;

uint8_t command_ring_pointer = 0;

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
    // xhci_set_iman_reg(0,3);
    xhci_set_iman_reg(0,1);
    xhci_set_iman_reg(0,2);
    if(usbsts&0b1000)
    {
        k_printf("xhci: event interrupt!\n");
        xhci_set_usbsts_reg(0b1000);
    }
    else if(usbsts&0b10000)
    {
        k_printf("xhci: portchange interrupt!\n");
        xhci_set_usbsts_reg(0b10000);
    }
    xhci_set_usbcmd_reg(5);
    xhci_set_imod_reg(0,0x00100FA0);
    interrupt_eoi();
    asm volatile ("sti");
}

DefaultTRB *xhci_request_free_command_trb(uint8_t inc)
{
    DefaultTRB *dtrb = (DefaultTRB*) (commandring + (sizeof(DefaultTRB)*command_ring_pointer));
    memset(dtrb,0,sizeof(DefaultTRB));
    if(inc)
    {
        command_ring_pointer++;
    }
    return dtrb;
}

CommandCompletionEventTRB *xhci_ring_and_wait(uint32_t doorbell_offset,uint32_t doorbell_value,uint32_t checkvalue)
{
    ((uint32_t*)(xhci_base_addr + xhci_doorbell_offset))[doorbell_offset] = doorbell_value;
    sleep(1);
    uint32_t timeout = 0;
    while((xhci_get_iman_reg(0)&1)==0)
    {
        sleep(1);
        if(timeout>100)
        {
            k_printf("\nxhci: panic: timeout, offset:%d value:%d check:%d \n",doorbell_offset,doorbell_value,checkvalue);
            return 0;
        }
        timeout++;
    }
    sleep(2);
    for(int i = 0 ; i < XHCI_EVENT_RING_SIZE ; i++)
    {
        CommandCompletionEventTRB *to = (CommandCompletionEventTRB*)&((CommandCompletionEventTRB*)(eventring+(i*sizeof(CommandCompletionEventTRB))))[0];
        if(to->DataBufferPointerLo==checkvalue)
        {
            return to;
        }
    }
    return 0;
}

uint8_t xhci_command_ring_get_switch()
{
    return 1;
}

uint8_t xhci_resultcode_explained(CommandCompletionEventTRB* res)
{
    const char * cc[] = {
        "Invalid",
        "Success",
        "Data Buffer Error",
        "Babble Detected Error",
        "USB Transaction Error",
        "TRB Error",
        "Stall Error",
        "Resource Error",
        "Bandwidth Error",
        "No Slots Available Error",
        "Invalid Stream Type Error",
        "Slot Not Enabled Error",
        "Endpoint Not Enabled Error",
        "Short Packet",
        "Ring Underrun",
        "Ring Overrun",
        "VF Event Ring Full Error",
        "Parameter Error",
        "Bandwidth Overrun Error",
        "Context State Error",
        "No Ping Response Error",
        "Event Ring Full Error",
        "Incompatible Device Error",
        "Missed Service Error",
        "Command Ring Stopped",
        "Command Aborted",
        "Stopped",
        "Stopped - Length Invalid",
        "Stopped - Short Packet",
        "Max Exit Latency Too Large Error",
        "Reserved",
        "Isoch Buffer Overrun",
        "Event Lost Error",
        "Undefined Error",
        "Invalid Stream ID Error",
        "Secondary Bandwidth Error",
        "Split Transaction Error"
    };
    k_printf("xhci: resultcode: (%x) %s \n",res->CompletionCode,cc[res->CompletionCode]);
    if(res->CompletionCode==1)
    {
        return res->SlotID;
    }
    else
    {
        return 0;
    }
}

uint8_t xhci_request_device_id()
{
    // Enable slot TRB
    EnableSlotCommandTRB *trb1 = (EnableSlotCommandTRB*) xhci_request_free_command_trb(1);
    trb1->CycleBit = xhci_command_ring_get_switch();
    trb1->TRBType = 9;

    EnableSlotCommandTRB *trb2 = (EnableSlotCommandTRB*) xhci_request_free_command_trb(0);
    trb2->CycleBit = xhci_command_ring_get_switch()==0?1:0;

    CommandCompletionEventTRB *res = xhci_ring_and_wait(0,0,(uint32_t)(upointer_t)trb1);
    if(res)
    {
        if(res->CompletionCode!=1)
        {
            return xhci_resultcode_explained(res);
        }
        return res->SlotID;
    }
    else
    {
        k_printf("xhci: couldent get xhci datatoken\n");
        return 0;
    }
}

uint8_t xhci_request_device_address(uint8_t device_id,void* data,uint8_t bsr )
{
    // Enable slot TRB
    SetAddressCommandTRB *trb1 = (SetAddressCommandTRB*) xhci_request_free_command_trb(1);
    trb1->CycleBit = xhci_command_ring_get_switch();
    trb1->TRBType = 11;
    trb1->BSR = bsr;
    trb1->DataBufferPointerLo = (uint32_t) (upointer_t) data;
    trb1->DataBufferPointerHi = 0;
    trb1->SlotID = device_id;

    SetAddressCommandTRB *trb2 = (SetAddressCommandTRB*) xhci_request_free_command_trb(0);
    trb2->CycleBit = xhci_command_ring_get_switch()==0?1:0;

    CommandCompletionEventTRB *res = xhci_ring_and_wait(0,0,(uint32_t)(upointer_t)trb1);
    if(res)
    {
        return xhci_resultcode_explained(res);
    }
    else
    {
        k_printf("xhci: couldent get xhci datatoken\n");
        return 0;
    }
}

void *xhci_request_device_descriptor(USBDevice *device)
{
    void* data = requestPage();

    SetupStageTRB *trb1 = (SetupStageTRB*) ((DefaultTRB*)(device->localring+(device->localringindex++*sizeof(DefaultTRB))));
    trb1->usbcmd.bRequestType = 0x80;
    trb1->usbcmd.bRequest = 6;
    trb1->usbcmd.wValue = 0x100;
    trb1->usbcmd.wIndex = 0;
    trb1->usbcmd.wLength = 8;
    trb1->TRBTransferLength = 8;
    trb1->InterrupterTarget = 0;
    trb1->Cyclebit = 1;
    trb1->ImmediateData = 1;
    trb1->TRBType = 2;
    trb1->TRT = 3;

    DataStageTRB *trb2 = (DataStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex++];
    trb2->Address1 = (uint32_t)(upointer_t) data;
    trb2->Address2 = 0;
    trb2->TRBTransferLength = 8;
    trb2->Cyclebit = 1;
    trb2->TRBType = 3;
    trb2->Direction = 1;

    StatusStageTRB *trb3 = (StatusStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex++];
    trb3->Cyclebit = 1;
    trb3->InterruptOnCompletion = 1;
    trb3->Direction = 1;
    trb3->TRBType = 4;

    StatusStageTRB *trb4 = (StatusStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex];
    trb4->Cyclebit = 0;

    CommandCompletionEventTRB *res = xhci_ring_and_wait(device->deviceaddres,1,(uint32_t)(upointer_t)trb3);
    if(res)
    {
        if(res->CompletionCode!=1)
        {
            xhci_resultcode_explained(res);
            return 0;
        }
        return data;
    }
    else
    {
        k_printf("xhci: couldent get xhci datatoken\n");
        return 0;
    }
}

void *xhci_request_device_configuration(USBDevice *device)
{
    uint32_t expectedsize = sizeof(usb_config_descriptor) + sizeof(usb_interface_descriptor) +(sizeof(EHCI_DEVICE_ENDPOINT)*2);
    void* data = requestPage();

    SetupStageTRB *trb1 = (SetupStageTRB*) ((DefaultTRB*)(device->localring+(device->localringindex++*sizeof(DefaultTRB))));
    trb1->usbcmd.bRequestType = 0x80;
    trb1->usbcmd.bRequest = 6;
    trb1->usbcmd.wValue = 0x200;
    trb1->usbcmd.wIndex = 0;
    trb1->usbcmd.wLength = expectedsize;
    trb1->TRBTransferLength = 8;
    trb1->InterrupterTarget = 0;
    trb1->Cyclebit = 1;
    trb1->ImmediateData = 1;
    trb1->TRBType = 2;
    trb1->TRT = 3;
    trb1->InterruptOnCompletion = 1;

    DataStageTRB *trb2 = (DataStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex++];
    trb2->Address1 = (uint32_t)(upointer_t) data;
    trb2->Address2 = 0;
    trb2->TRBTransferLength = expectedsize;
    trb2->Cyclebit = 1;
    trb2->TRBType = 3;
    trb2->Direction = 1;
    trb2->InterruptOnCompletion = 1;

    StatusStageTRB *trb3 = (StatusStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex++];
    trb3->Cyclebit = 1;
    trb3->InterruptOnCompletion = 1;
    trb3->Direction = 1;
    trb3->TRBType = 4;

    StatusStageTRB *trb4 = (StatusStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex];
    trb4->Cyclebit = 0;

    CommandCompletionEventTRB *res = xhci_ring_and_wait(device->deviceaddres,1,(uint32_t)(upointer_t)trb3);
    if(res)
    {
        if(res->CompletionCode!=1)
        {
            xhci_resultcode_explained(res);
            return 0;
        }
        return data;
    }
    else
    {
        k_printf("xhci: couldent get xhci datatoken\n");
        return 0;
    }
}

uint8_t xhci_request_set_config(USBDevice *device,uint8_t configid)
{
    SetupStageTRB *trb1 = (SetupStageTRB*) ((DefaultTRB*)(device->localring+(device->localringindex++*sizeof(DefaultTRB))));
    trb1->usbcmd.bRequest = USB2_REQUEST_SET_CONFIGURATION;
    trb1->usbcmd.wValue = configid;
    trb1->usbcmd.wIndex = 0;
    trb1->usbcmd.wLength = 0;
    trb1->TRBTransferLength = 8;
    trb1->InterrupterTarget = 0;
    trb1->Cyclebit = 1;
    trb1->ImmediateData = 1;
    trb1->TRBType = 2;
    trb1->TRT = 3;

    StatusStageTRB *trb3 = (StatusStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex++];
    trb3->Cyclebit = 1;
    trb3->InterruptOnCompletion = 1;
    trb3->Direction = 1;
    trb3->TRBType = 4;

    CommandCompletionEventTRB *res = xhci_ring_and_wait(device->deviceaddres,1,(uint32_t)(upointer_t)trb3);
    if(res)
    {
        if(res->CompletionCode!=1)
        {
            return xhci_resultcode_explained(res);
        }
        return 1;
    }
    else
    {
        k_printf("xhci: couldent get xhci datatoken\n");
        return 0;
    }
}

uint8_t xhci_request_configure_endpoint_command(USBDevice *device,void *data)
{
    // Enable slot TRB
    ConfigureEndpointCommandTRB *trb1 = (ConfigureEndpointCommandTRB*) xhci_request_free_command_trb(1);
    trb1->CycleBit = xhci_command_ring_get_switch();
    trb1->TRBType = 12;
    trb1->DataBufferPointerLo = ((uint32_t) (upointer_t) data);//2
    trb1->DataBufferPointerHi = 0;
    trb1->Deconfigure = 0;
    trb1->SlotID = device->deviceaddres;

    ConfigureEndpointCommandTRB *trb2 = (ConfigureEndpointCommandTRB*) xhci_request_free_command_trb(0);
    trb2->CycleBit = xhci_command_ring_get_switch()==0?1:0;

    CommandCompletionEventTRB *res = xhci_ring_and_wait(0,0,(uint32_t)(upointer_t)trb1);
    if(res)
    {
        if(res->CompletionCode!=1)
        {
            return xhci_resultcode_explained(res);
        }
        return res->SlotID;
    }
    else
    {
        k_printf("xhci: couldent get xhci datatoken\n");
        return 0;
    }
}

void* xhci_setup_endpoint(USBDevice *device,EHCI_DEVICE_ENDPOINT *endpoint,uint8_t id,XHCIInputContextBuffer *backup)
{
    XHCIInputContextBuffer *infostructures = requestPage();
    void *localring = requestPage();
    memset(infostructures,0,sizeof(XHCIInputContextBuffer));
    memset(localring,0,0x1000);
    memcpy(infostructures,backup,sizeof(XHCIInputContextBuffer));
    infostructures->icc.Dregisters = 0;
    infostructures->icc.Aregisters = (1<<(id+1));
    infostructures->icc.ConfigurationValue = device->config->bConfigurationValue;
    infostructures->slotcontext.ContextEntries = id+2;
    infostructures->epx[id-1].EPType = endpoint->bEndpointAddress&0x80?6:2;
    infostructures->epx[id-1].MaxPacketSize = endpoint->wMaxPacketSize & 0x3FFF;
    infostructures->epx[id-1].TRDequeuePointerLow = ((uint32_t) (upointer_t) localring)>>4;
    infostructures->epx[id-1].DequeueCycleState = 1;

    uint8_t info = xhci_request_configure_endpoint_command(device,infostructures);
    if(info==1){

        // lets check if the ring is working or not...
        DefaultTRB *trb1 = (DefaultTRB*) ((DefaultTRB*)(localring+(0*sizeof(DefaultTRB))));
        trb1->Cyclebit = 1;
        trb1->TRBType = 8;
        trb1->InterruptOnCompletion = 1;

        StatusStageTRB *trb4 = (StatusStageTRB*) & ((DefaultTRB*)localring)[1];
        trb4->Cyclebit = 0;

        CommandCompletionEventTRB *res = xhci_ring_and_wait(device->deviceaddres,id+1,(uint32_t)(upointer_t)trb1);
        if(!res){
            return 0;
        }
        if(res->CompletionCode!=1){
            return 0;
        }
        return localring;
    }
    return 0;
}

void *xhci_request_normal_data(USBDevice *device, uint8_t request, uint8_t dir, uint8_t type, uint8_t recieve, uint16_t windex,uint16_t wlength, uint16_t wvalue,uint8_t size)
{
    void* data = requestPage();

    SetupStageTRB *trb1 = (SetupStageTRB*) ((DefaultTRB*)(device->localring+(device->localringindex++*sizeof(DefaultTRB))));
    trb1->usbcmd.bRequest = request;
    trb1->usbcmd.bRequestType = 0;
    trb1->usbcmd.bRequestType |= dir; // dir is out
    trb1->usbcmd.bRequestType |= (type<<5); // type is standard
    trb1->usbcmd.bRequestType |= recieve; // recieve 
    trb1->usbcmd.wValue = wvalue;
    trb1->usbcmd.wIndex = windex;
    trb1->usbcmd.wLength = wlength;
    trb1->TRBTransferLength = 8;
    trb1->InterrupterTarget = 0;
    trb1->Cyclebit = 1;
    trb1->ImmediateData = 1;
    trb1->TRBType = 2;
    trb1->TRT = 3;
    trb1->InterruptOnCompletion = 1;

    DataStageTRB *trb2 = (DataStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex++];
    trb2->Address1 = (uint32_t)(upointer_t) data;
    trb2->Address2 = 0;
    trb2->TRBTransferLength = wlength;
    trb2->Cyclebit = 1;
    trb2->TRBType = 3;
    trb2->Direction = 1;
    trb2->InterruptOnCompletion = 1;

    StatusStageTRB *trb3 = (StatusStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex++];
    trb3->Cyclebit = 1;
    trb3->InterruptOnCompletion = 1;
    trb3->Direction = 1;
    trb3->TRBType = 4;

    StatusStageTRB *trb4 = (StatusStageTRB*) & ((DefaultTRB*)device->localring)[device->localringindex];
    trb4->Cyclebit = 0;

    CommandCompletionEventTRB *res = xhci_ring_and_wait(device->deviceaddres,1,(uint32_t)(upointer_t)trb3);
    if(res)
    {
        if(res->CompletionCode!=1)
        {
            xhci_resultcode_explained(res);
            return 0;
        }
        return data;
    }
    else
    {
        k_printf("xhci: couldent get xhci datatoken\n");
        return 0;
    }
}

uint8_t xhci_send_bulk_data(USBDevice *device, uint8_t address,uint32_t command,int8_t endpoint,int8_t size)
{

    DefaultTRB *trb1 = (DefaultTRB*) ((DefaultTRB*)(device->localringout+(device->localringindexout++*sizeof(DefaultTRB))));
    trb1->DataBufferPointerLo = command;
    trb1->TRBTransferLength = size;
    trb1->InterrupterTarget = 0;
    trb1->Cyclebit = 1;
    trb1->ImmediateData = 0;
    trb1->TRBType = 1;
    trb1->InterruptOnCompletion = 1;

    StatusStageTRB *trb3 = (StatusStageTRB*) & ((DefaultTRB*)device->localringout)[device->localringindexout++];
    trb3->Cyclebit = 1;
    trb3->InterruptOnCompletion = 1;
    trb3->Direction = 1;
    trb3->TRBType = 4;

    StatusStageTRB *trb4 = (StatusStageTRB*) & ((DefaultTRB*)device->localringout)[device->localringindexout];
    trb4->Cyclebit = 0;

    CommandCompletionEventTRB *res = xhci_ring_and_wait(device->deviceaddres,device->localringoutid,(uint32_t)(upointer_t)trb3);
    if(res)
    {
        return res->CompletionCode;
    }
    else
    {
        k_printf("xhci: couldent get xhci datatoken\n");
        return 0;
    }
}

void *xhci_recieve_bulk_data(USBDevice *device, uint8_t address,uint8_t endpoint,uint32_t size,uint8_t toggle)
{
    k_printf("xhci: debug: recieve bulk data\n");for(;;);
}

void xhci_port_install(uint8_t portid)
{
    uint32_t portc = xhci_get_portsc_reg(portid);

    if(portc&2)
    {
        k_printf("xhci: port %d is a USB3.0 connection!\n",portid);
    }
    else
    {
        return;
    }

    portc = xhci_get_portsc_reg(portid);
    if((portc&3)!=3)
    {
        k_printf("xhci: port %d is not ready yet!\n",portid);
        return;
    }
    
    uint8_t portspeed = (portc >> 10) & 0b111;
    uint16_t calculatedportspeed = 0;
    if(portspeed==4)
    {
        k_printf("xhci: port %d is a superspeed connection!\n",portid);
        calculatedportspeed = 512;
    }
    else if(portspeed==3)
    {
        k_printf("xhci: port %d is a highspeed connection!\n",portid);
        calculatedportspeed = 64;
    }
    else
    {
        k_printf("xhci: port %d , unknown portspeed: %d \n",portid,portspeed);
        return;
    }

    // request next free device-id
    uint8_t deviceid = xhci_request_device_id();
    if(deviceid==0)
    {
        goto failure;
    }
    sleep(1);
    portc = xhci_get_portsc_reg(portid);
    k_printf("xhci: slotid %d has now a deviceid of %d portstate %d and speed of %d , ps: %x \n",portid,deviceid,(portc>>5)&0b1111,portspeed,portc);

    // request a device address for our device!
    XHCIInputContextBuffer *infostructures = requestPage();
    void *localring = requestPage();
    memset(infostructures,0,sizeof(XHCIInputContextBuffer));
    memset(localring,0,0x1000);
    // first, fill dcbaap with our values...
    ((uint32_t*)dcbaap)[(deviceid*2)+0] = ((uint32_t)(upointer_t)(infostructures+64));
    ((uint32_t*)dcbaap)[(deviceid*2)+1] = 0;
    // then the rest of all the info...
    infostructures->icc.Aregisters = 1;
    infostructures->slotcontext.RootHubPortNumber = portid;
    infostructures->slotcontext.ContextEntries = 1;
    infostructures->slotcontext.Speed = portspeed;
    infostructures->epc.LSA = 0;
    infostructures->epc.EPType = 4;
    infostructures->epc.Cerr = 3;
    infostructures->epc.MaxPacketSize = calculatedportspeed;
    infostructures->epc.TRDequeuePointerLow = ((uint32_t) (upointer_t) localring)>>4 ;
    infostructures->epc.DequeueCycleState = 1;
    infostructures->epc.AverageTRBLength = 8;

    // now do the call
    if(xhci_request_device_address(deviceid,infostructures,0)==0)
    {
        goto failure;
    }
    k_printf("xhci: port has now a address!\n");

    // this seems to go well....
    USBDevice *device = getFreeUSBDeviceClass();
    device->physport = portid;
    device->protocol = 3;
    device->deviceaddres = deviceid;
    device->localring = localring;
    device->localringindex = 0;

    // fetch some data to test...
    uint8_t* did = (uint8_t*)xhci_request_device_descriptor(device);
    if(did==0)
    {
        goto failure;
    }
    k_printf("xhci: first port communication went well!\n");

    // get some advanced configurations
    uint8_t* cinforaw = (uint8_t*)xhci_request_device_configuration(device);
    if(cinforaw==0)
    {
        goto failure;
    }
    usb_interface_descriptor* desc = (usb_interface_descriptor*)(((unsigned long)cinforaw)+sizeof(usb_config_descriptor));
    EHCI_DEVICE_ENDPOINT *ep1 = (EHCI_DEVICE_ENDPOINT*)(((unsigned long)cinforaw)+sizeof(usb_config_descriptor)+sizeof(usb_interface_descriptor));
    EHCI_DEVICE_ENDPOINT *ep2 = (EHCI_DEVICE_ENDPOINT*)(((unsigned long)cinforaw)+sizeof(usb_config_descriptor)+sizeof(usb_interface_descriptor)+13);

    device->config = (usb_config_descriptor*) cinforaw;
    device->interface = (usb_interface_descriptor*) (((unsigned long)cinforaw)+sizeof(usb_config_descriptor));
    device->ep1 = (EHCI_DEVICE_ENDPOINT*)(((unsigned long)cinforaw)+sizeof(usb_config_descriptor)+sizeof(usb_interface_descriptor));
    device->ep2 = (EHCI_DEVICE_ENDPOINT*)(((unsigned long)cinforaw)+sizeof(usb_config_descriptor)+sizeof(usb_interface_descriptor)+13);
    device->localringid = 1;
    if(ep1->bEndpointAddress&0x80){
        device->epINid = ep1->bEndpointAddress&0xF;
        device->localringinid = 2;
        device->epOUTid = ep2->bEndpointAddress&0xF;
        device->localringoutid = 3;
    }else{
        device->epINid = ep2->bEndpointAddress&0xF;
        device->localringinid = 3;
        device->epOUTid = ep1->bEndpointAddress&0xF;
        device->localringoutid = 2;
    }

    k_printf("xhci: port %d has a class of %d and a sublclass of %d \n",portid,desc->bInterfaceClass,desc->bInterfaceSubClass);
    k_printf("xhci: port %d bNumEndpoints:%d bNumInterfaces:%d \n",portid,desc->bNumEndpoints,device->config->bNumInterfaces);
    if(desc->bNumEndpoints==2&&device->config->bNumInterfaces==1)
    {
        k_printf("xhci: port %d situation as expected\n",portid);

        void *ep_ring_1 = xhci_setup_endpoint(device,ep1,1,infostructures);
        void *ep_ring_2 = xhci_setup_endpoint(device,ep2,2,infostructures);

        if(ep_ring_1==0||ep_ring_2==0)
        {
            k_printf("xhci: port %d unable to create valid endpoints\n",portid);
            goto failure;
        }
        k_printf("xhci: port %d endpoint 1 ring at %x \n",portid,ep_ring_1);
        k_printf("xhci: port %d endpoint 2 ring at %x \n",portid,ep_ring_2);

        uint8_t littleresult = xhci_request_set_config(device,1);
        if(littleresult==0)
        {
            goto failure;
        }

        k_printf("xhci: port %d device accepted set configuration!\n",portid);

        if(ep1->bEndpointAddress&0x80)
        {
            device->localringin = ep_ring_1;
            device->localringout = ep_ring_2;
        }
        else
        {
            device->localringin = ep_ring_2;
            device->localringout = ep_ring_1;
        }
        device->localringindexin = 1;
        device->localringindexout = 1;
    }
    else
    {
        k_printf("xhci: port %d cannot handle this situation\n",portid);
        goto failure;
    }

    install_usb_device(device);
    return;

    failure:
    k_printf("xhci: The installing of port %d failed misserably!\n",portid);
}

void xhci_driver_start(int bus,int slot,int function)
{
    unsigned short vendor = pciConfigReadWord(bus,slot,function,0);
    k_printf("xhci: Driver started at bus %d slot %d and function %d and vendor %d\n",bus,slot,function,vendor);

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
    if(hcinterfaceversion!=0x100&&hcinterfaceversion!=0)
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
    k_printf("xhci: capability pointer at %x \n",capapointer);
    void *cappointer = capapointer + xhci_base_addr;
    int captimeout = 100;
    while(1)
    {
        volatile uint32_t reg = ((volatile uint32_t*)cappointer)[0];
        uint8_t capid = reg & 0xFF;
        uint8_t capoffset = (reg>>8) & 0xFF;
        if(capid==0)
        {
            break;
        }
        if( capid==1 && reg & 0x10000 )
        {
            k_printf("xhci: found usblegacy stuff\n");
            ((volatile uint32_t*)cappointer)[0] |= 0x1000000;
            sleep(1);
            captimeout--;
            if(captimeout==0)
            {
                break;
            }
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

    //
    // Enable busmastering
    pci_enable_busmastering_when_needed(bus,slot,function);

    // lets trigger a reset.
    // send reset sequence
    k_printf("xhci: about to reset!\n");
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
    xhci_set_crcr_reg((uint32_t)((upointer_t)commandring)|1);

    // Allocate and initialize the Event Ring Segment(s).
    eventring = requestPage();
    memset(eventring,0,XHCI_EVENT_RING_SIZE * sizeof(CommandCompletionEventTRB));

    // Allocate the Event Ring Segment Table (ERST)
    // (section 6.5). Initialize ERST table entries to point
    // to and to define the size (in TRBs) of the respective
    // Event Ring Segment.
    segmenttable = (XHCIEventRingSegmentTable*) requestPage();
    memset(segmenttable,0,sizeof(XHCIEventRingSegmentTable));
    segmenttable->ring_segment_base_address_low = (uint32_t)(upointer_t)eventring;
    segmenttable->ring_segment_size = XHCI_EVENT_RING_SIZE;

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

    // Enable the Interrupter by writing a ‘1’ to the
    // Interrupt Enable (IE) field of the Interrupter
    // Management register (5.5.2.1).
    xhci_set_iman_reg(0,2);

    // Set all pointers to zero
    command_ring_pointer = 0;

    // Write the USBCMD (5.4.1) to turn the host controller ON via setting the
    // Run/Stop (R/S) bit to ‘1’. This operation allows the xHC to begin
    // accepting doorbell references.
    #ifdef ENABLE_INTERRUPTS
    xhci_set_usbcmd_reg(5);
    sleep(20);
    for(int i = 0 ; i < XHCI_EVENT_RING_SIZE ; i++)
    {
        PortStatusChangeEventTRB *to = (PortStatusChangeEventTRB*)&((PortStatusChangeEventTRB*)(eventring+(i*sizeof(PortStatusChangeEventTRB))))[0];
        if(to->TRBType==34)
        {
            // k_printf("xhci: %d - type:%d portid:%d \n",i,to->TRBType,to->PortID);
            xhci_set_erdp_reg(0,(uint32_t)(upointer_t)to);
            xhci_port_install(to->PortID);
        }
    }
    #else
    xhci_set_usbcmd_reg(1);
    sleep(10);
    for(uint8_t i = 1 ; i < xhci_number_of_ports ; i++){
        xhci_port_install(i);
    }
    #endif 
    for(;;);
}