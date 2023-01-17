#pragma once 
#include <stdint.h>

#define EHCI_PCI_INFO_REG           0x60
#define EHCI_PROTOCOL_VER           0x20
#define EHCI_HCI_VER                0x100
#define EHCI_PERIODIC_FRAME_SIZE    1024

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