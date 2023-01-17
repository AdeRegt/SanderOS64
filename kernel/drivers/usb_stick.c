#include "../include/usb.h"
#include "../include/graphics.h"

void install_usb_stick(USBDevice *device)
{
    k_printf("usb-%d: found a USB Mass Storage Driver!\n",device->physport);for(;;);
}