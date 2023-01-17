#include "../include/usb.h"
#include "../include/graphics.h"

#define USB_STICK_REQUEST_LUN 0xFE

void install_usb_stick(USBDevice *device)
{
    k_printf("usb-%d: found a USB Mass Storage Driver!\n",device->physport);
    if(device->interface->bInterfaceSubClass!=USB_IF_CLASS_MSD_SCSI){
        k_printf("usb-%d: Unknown protocol type\n",device->physport);
        return;
    }
    uint8_t* luns = ehci_request_normal_data(USB_STICK_REQUEST_LUN,0x80,1,1,0,1,0,1,device->deviceaddres);
    if(luns==0){
        k_printf("usb-%d: cant get lun!\n",device->physport);
    }
    uint8_t lun = luns[0];
    k_printf("usb-%d: LUN is %d \n",device->physport,lun);
    for(;;);
}