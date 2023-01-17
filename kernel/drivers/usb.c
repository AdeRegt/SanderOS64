#include "../include/usb.h"
#include "../include/graphics.h"

USBDevice usbpool[10];
uint8_t usbpool_i = 0;

USBDevice *getFreeUSBDeviceClass(){
    return (USBDevice*) &usbpool[usbpool_i++];
}

void install_usb_device(USBDevice *device){
    if(device->interface->bInterfaceClass==0x08&&device->interface->bInterfaceSubClass==0x06)
    {
        k_printf("usb-%d: found a USB Mass Storage Driver!\n",device->physport);for(;;);
    }
}