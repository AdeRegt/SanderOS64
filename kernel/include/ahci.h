#pragma once 
#include <stdint.h>
#include "outint.h"

typedef struct{
    uint32_t commandListBase;
    uint32_t commandListBaseUpper;
    uint32_t fisBaseAddress;
    uint32_t fisBaseAddressUpper;
    uint32_t interruptStatus;
    uint32_t interruptEnable;
    uint32_t cmdSts;
    uint32_t rsv0;
    uint32_t taskFileData;
    uint32_t signature;
    uint32_t sataStatus;
    uint32_t sataControl;
    uint32_t sataError;
    uint32_t sataActive;
    uint32_t commandIssue;
    uint32_t sataNotification;
    uint32_t fisSwitchControl;
    uint32_t rsv1[11];
    uint32_t vendor[4];
}__attribute__((packed)) HBAPort;

typedef struct{
    uint32_t hostCapability;
    uint32_t globalHostControl;
    uint32_t interruptStatus;
    uint32_t portsImplemented;
    uint32_t version;
    uint32_t cccControl;
    uint32_t cccPorts;
    uint32_t enclosureManagementLocation;
    uint32_t enclosureManagementControl;
    uint32_t hostCapabilitiesExtended;
    uint32_t biosHandoffCtrlSts;
    uint8_t rsv0[0x74];
    uint8_t vendor[0x60];
    HBAPort ports[1];
}__attribute__((packed)) HBAMemory;

typedef struct {
    uint8_t commandFISLength:5;
    uint8_t atapi:1;
    uint8_t write:1;
    uint8_t prefetchable:1;

    uint8_t reset:1;
    uint8_t bist:1;
    uint8_t clearBusy:1;
    uint8_t rsv0:1;
    uint8_t portMultiplier:4;

    uint16_t prdtLength;
    uint32_t prdbCount;
    uint32_t commandTableBaseAddress;
    uint32_t commandTableBaseAddressUpper;
    uint32_t rsv1[4];
}__attribute__((packed)) HBACommandHeader;

typedef struct{
    uint32_t dataBaseAddress;
    uint32_t dataBaseAddressUpper;
    uint32_t rsv0;

    uint32_t byteCount:22;
    uint32_t rsv1:9;
    uint32_t interruptOnCompletion:1;
}__attribute__((packed)) HBAPRDTEntry;

typedef struct{
    uint8_t commandFIS[64];

    uint8_t atapiCommand[16];

    uint8_t rsv[48];

    HBAPRDTEntry prdtEntry[];
}__attribute__((packed)) HBACommandTable;

typedef struct {
    uint8_t fisType;

    uint8_t portMultiplier:4;
    uint8_t rsv0:3;
    uint8_t commandControl:1;

    uint8_t command;
    uint8_t featureLow;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t deviceRegister;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t featureHigh;

    uint8_t countLow;
    uint8_t countHigh;
    uint8_t isoCommandCompletion;
    uint8_t control;

    uint8_t rsv1[4];
}__attribute__((packed)) FIS_REG_H2D;

enum FIS_TYPE{
    FIS_TYPE_REG_H2D = 0x27,
    FIS_TYPE_REG_D2H = 0x34,
    FIS_TYPE_DMA_ACT = 0x39,
    FIS_TYPE_DMA_SETUP = 0x41,
    FIS_TYPE_DATA = 0x46,
    FIS_TYPE_BIST = 0x58,
    FIS_TYPE_PIO_SETUP = 0x5F,
    FIS_TYPE_DEV_BITS = 0xA1,
};

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000

void initialise_ahci_driver(unsigned long bar, unsigned long ints);