#include "../include/usb.h"
#include "../include/graphics.h"
#include "../include/memory.h"
#include "../include/device.h"
#include "../include/fs/fat.h"

#define USB_STICK_REQUEST_LUN 0xFE

typedef struct {
    uint32_t signature;
    uint32_t tag;
    uint32_t transferlength;
    uint8_t flags;
    uint8_t lun;
    uint8_t command_len;
    uint8_t data[16];
} __attribute__ ((packed)) CommandBlockWrapper;

typedef struct {
    uint32_t signature;
    uint32_t tag;
    uint32_t data_residue;
    uint8_t status;
} __attribute__ ((packed)) CommandStatusWrapper;

typedef struct {
    uint8_t periphal_device_type:5;
    uint8_t peripheral_qualifier:3;
    uint8_t reserved1:7;
    uint8_t RMB:1;
    uint8_t version;
} __attribute__ ((packed)) InquiryBlock;

void *usb_stick_send_request(USBDevice *device, CommandBlockWrapper *cbw)
{
    uint8_t g = usb_send_bulk_data(device,device->deviceaddres,(uint32_t)(upointer_t)cbw,device->epOUTid,sizeof(CommandBlockWrapper) );
    if(g==0)
    {
        k_printf("__block: datablok0 failed!\n");
        return 0;
    }
    void* res = usb_recieve_bulk_data(device,device->deviceaddres,device->epINid,cbw->transferlength,0);
    if(res==0)
    {
        k_printf("__block: datablok1 failed!\n");
        return 0;
    }
    uint32_t oc = (cbw->transferlength/512)&2;
    CommandStatusWrapper *cq = usb_recieve_bulk_data(device,device->deviceaddres,device->epINid,13,oc==2?0:1);
    if(cq==0)
    {
        k_printf("__block: csw failed!\n");
        return 0;
    }
    if(cq->signature!=0x53425355)
    {
        k_printf("__commandstatuswrapper: invalidsignature\n");
        return 0;
    }
    if(cq->tag!=cbw->tag)
    {
        k_printf("__commandstatuswrapper: invalidtag\n");
        return 0;
    }
    if(cq->data_residue)
    {
        k_printf("__commandstatuswrapper: dataresidue not null but %d \n",cq->data_residue);
        return 0;
    }
    if(cq->status)
    {
        k_printf("__commandstatuswrapper: status not null but %d \n",cq->status);
        return 0;
    }
    return res;
}

uint32_t usbtagpointer = 1;

CommandBlockWrapper* usb_stick_generate_pointer()
{
    CommandBlockWrapper *ep = (CommandBlockWrapper*) requestPage();
    memset(ep,0,sizeof(CommandBlockWrapper));
    ep->signature = 0x43425355;
    ep->tag = usbtagpointer++;
    return ep;
}

uint8_t usb_stick_read(Blockdevice* dev, upointer_t sector, uint32_t counter, void* buffer)
{
    CommandBlockWrapper *ep = usb_stick_generate_pointer();
    ep->transferlength = 512 * counter;
    ep->flags = 0x80;
    ep->command_len = 10;
    // command READ(0x12)
    ep->data[0] = 0x28;
    // reserved
    ep->data[1] = 0;
    // lba
    ep->data[2] = (uint8_t) ((sector >> 24) & 0xFF);
    ep->data[3] = (uint8_t) ((sector >> 16) & 0xFF);
    ep->data[4] = (uint8_t) ((sector >> 8) & 0xFF);
    ep->data[5] = (uint8_t) ((sector) & 0xFF);
    // counter
    ep->data[6] = 0;
    ep->data[7] = (uint8_t) ((counter >> 8) & 0xFF);
    ep->data[8] = (uint8_t) ((counter) & 0xFF);

    uint8_t* cq = usb_stick_send_request((USBDevice*)dev->attachment,ep);
    freePage(ep);
    if(cq==0){
        return 0;
    }
    memcpy((void*)buffer,cq,512*counter);
    freePage(cq);
    return 1;
}

uint8_t usb_stick_write(Blockdevice* dev, upointer_t sector, uint32_t counter, void* buffer){
    return 0; // writes are not supported, we are read only!
}

void install_usb_stick(USBDevice *device)
{
    k_printf("usb-%d: found a USB Mass Storage Driver!\n",device->physport);
    if(device->interface->bInterfaceSubClass!=USB_IF_CLASS_MSD_SCSI){
        k_printf("usb-%d: Unknown protocol type\n",device->physport);
        return;
    }
    uint8_t* luns = usb_request_normal_data(device,USB_STICK_REQUEST_LUN,0x80,1,1,0,1,0,1);
    if(luns==0){
        k_printf("usb-%d: cant get lun!\n",device->physport);
        return;
    }
    uint8_t lun = luns[0];

    Blockdevice* bdev = registerBlockDevice(512, usb_stick_read, usb_stick_write, 0, device);

    void *cq = requestPage();
    uint8_t g = usb_stick_read(bdev,0,1,cq);
    if(g){
        fat_detect_and_initialise(bdev,cq);
    }
}