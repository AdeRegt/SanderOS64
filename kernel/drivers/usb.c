#include "../include/usb.h"
#include "../include/graphics.h"

USBDevice usbpool[10];
uint8_t usbpool_i = 0;

USBDevice *getFreeUSBDeviceClass(){
    return (USBDevice*) &usbpool[usbpool_i++];
}

void install_usb_device(USBDevice *device){
    if(device->interface->bInterfaceClass==USB_IF_CLASS_MSD&&device->interface->bInterfaceSubClass==0x06)
    {
        install_usb_stick(device);
    }
}