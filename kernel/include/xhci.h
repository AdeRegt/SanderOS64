#pragma once 
#include <stdint.h>

void initialise_xhci_driver(unsigned long addr);

typedef struct{
    uint8_t caplength;
    uint8_t rsvd;
    uint16_t hciversion;
    uint32_t hcsparams1;
    uint32_t hcsparams2;
    uint32_t hcsparams3;
    uint32_t hccparams1;
    uint32_t dboff;
    uint32_t rtsoff;
    uint32_t hccparams2;
}__attribute__((packed)) XHCICapabilityRegisters;

typedef struct{
    uint32_t portsc;
    uint32_t portpmsc;
    uint32_t portli;
    uint32_t porthlpmc;
}__attribute__((packed)) XHCIPortRegister;

typedef struct{
    uint32_t usbcmd;
    uint32_t usbsts;
    uint32_t pagesize;
    uint8_t rsvdA[0x08];
    uint32_t dnctrl;
    uint64_t crcr;
    uint8_t rsvdB[0x10];
    uint64_t dcbaap;
    uint32_t config;
    uint8_t rsvdC[0x3C4];
    XHCIPortRegister ports[30];
}__attribute__((packed)) XHCIOperationalRegisters;