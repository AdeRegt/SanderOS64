#pragma once 
#include <stdint.h>

typedef struct{
    uint32_t size;
    uint32_t version;
    uint32_t type;
    uint32_t rc;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t flags;
    uint32_t result;
} __attribute__((packed)) VMMDevHGCMRequestHeader;

typedef struct{
    VMMDevHGCMRequestHeader header;
    uint32_t type;
    uint8_t location[128];
    uint32_t clientId;
} __attribute__((packed)) VMMDevHGCMConnect;

typedef struct {
    uint32_t size;
    uint32_t version;
    uint32_t type;
    uint32_t rc;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t version2;
    uint32_t ostype;
} __attribute__((packed)) vbox_guest_info;