#pragma once 
#include <stdint.h>

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

#define USB_IF_CLASS_MSD 8
#define USB_IF_CLASS_MSD_SCSI 6


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

typedef struct  {
    uint8_t bRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} EhciCMD;

typedef struct {
    uint8_t deviceaddres;
    uint8_t protocol;
    usb_config_descriptor *config;
    usb_interface_descriptor *interface;
    EHCI_DEVICE_ENDPOINT *ep1;
    EHCI_DEVICE_ENDPOINT *ep2;
    uint8_t physport;
} USBDevice;

USBDevice *getFreeUSBDeviceClass();
void install_usb_device(USBDevice *device);
void install_usb_stick(USBDevice *device);
void *ehci_request_normal_data(uint8_t request, uint8_t dir, uint8_t type, uint8_t recieve, uint16_t windex,uint16_t wlength, uint16_t wvalue,uint8_t size,uint8_t address);
