#include "../include/usb.h"
#include "../include/graphics.h"

USBDevice usbpool[10];
uint8_t usbpool_i = 0;

void *usb_request_normal_data(USBDevice *device, uint8_t request, uint8_t dir, uint8_t type, uint8_t recieve, uint16_t windex,uint16_t wlength, uint16_t wvalue,uint8_t size,uint8_t address)
{
    if(device->protocol==2)
    {
        return ehci_request_normal_data(request,dir,type,recieve,windex,wlength,wvalue,size,address);
    }
    else
    {
        k_printf("usb: function usb_request_normal_data is not available for this device type!\n");
        return 0;
    }
}

void usb_send_bulk_data(USBDevice *device, uint8_t address,uint32_t command,int8_t endpoint,int8_t size)
{
    if(device->protocol==2)
    {
        ehci_send_bulk_data(address,command,endpoint,size);
    }
    else
    {
        k_printf("usb: function usb_send_bulk_data is not available for this device type!\n");
    }
}

void *usb_recieve_bulk_data(USBDevice *device, uint8_t address,uint8_t endpoint,uint16_t size,uint8_t toggle)
{
    if(device->protocol==2)
    {
        return ehci_recieve_bulk_data(address,endpoint,size,toggle);
    }
    else
    {
        k_printf("usb: function usb_recieve_bulk_data is not available for this device type!\n");
        return 0;
    }
}

USBDevice *getFreeUSBDeviceClass()
{
    return (USBDevice*) &usbpool[usbpool_i++];
}

void install_usb_device(USBDevice *device){
    if(device->interface->bInterfaceClass==USB_IF_CLASS_MSD)
    {
        install_usb_stick(device);
    }
    else
    {
        k_printf("usb: unknown device class: %d \n",device->interface->bInterfaceClass);
    }
}