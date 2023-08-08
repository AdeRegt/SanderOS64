#pragma once 
#include <stdint.h>


#define XHCI_EVENT_RING_SIZE 32

typedef struct{
    uint32_t ring_segment_base_address_low;
    uint32_t ring_segment_base_address_high;
    uint16_t ring_segment_size;
    uint16_t reservedA;
    uint32_t reservedB;
}__attribute__((packed)) XHCIEventRingSegmentTable;

typedef struct{
    uint32_t DataBufferPointerLo;
    uint32_t DataBufferPointerHi;
    uint32_t CommandCompletionParameter:24;
    uint16_t CompletionCode:8;
    uint8_t C:1;
    uint16_t reserved3:9;
    uint8_t TRBType:6;
    uint32_t VFID:8;
    uint32_t SlotID:8;
}__attribute__((packed)) CommandCompletionEventTRB;

typedef struct{
    uint32_t reserved1:24;
    uint16_t PortID:8;
    uint32_t reserved2;
    uint32_t reserved3:24;
    uint16_t CompletionCode:8;
    uint8_t C:1;
    uint16_t reserved4:9;
    uint8_t TRBType:6;
    uint32_t reserved5:16;
}__attribute__((packed)) PortStatusChangeEventTRB;

typedef struct{
    uint32_t DataBufferPointerLo;
    uint32_t DataBufferPointerHi;
    uint32_t TRBTransferLength:17;
    uint16_t TDSize:5;
    uint16_t InterrupterTarget:10;
    uint16_t Cyclebit:1;
    uint16_t EvaluateNextTRB:1;
    uint16_t InterruptonShortPacket:1;
    uint16_t NoSnoop:1;
    uint16_t Chainbit:1;
    uint16_t InterruptOnCompletion:1;
    uint16_t ImmediateData:1;
    uint16_t RsvdZ1:2;
    uint16_t BlockEventInterrupt:1;
    uint16_t TRBType:6;
    uint16_t RsvdZ2:16;
}__attribute__((packed))DefaultTRB;

typedef struct{
    EhciCMD usbcmd;
    uint32_t TRBTransferLength:17;
    uint16_t Reserved1:5;
    uint16_t InterrupterTarget:10;
    uint16_t Cyclebit:1;
    uint16_t Reserved2:4;
    uint16_t InterruptOnCompletion:1;
    uint16_t ImmediateData:1;
    uint16_t Reserved3:3;
    uint16_t TRBType:6;
    uint16_t TRT:2;
    uint16_t RsvdZ2:14;
}__attribute__((packed))SetupStageTRB;

typedef struct{
    uint32_t Address1;
    uint32_t Address2;
    uint32_t TRBTransferLength:17;
    uint16_t TDSize:5;
    uint16_t InterrupterTarget:10;
    uint16_t Cyclebit:1;
    uint16_t EvaluateNextTRB:1;
    uint16_t InterruptonShortPacket:1;
    uint16_t NoSnoop:1;
    uint16_t Chainbit:1;
    uint16_t InterruptOnCompletion:1;
    uint16_t ImmediateData:1;
    uint16_t Reserved3:3;
    uint16_t TRBType:6;
    uint16_t Direction:1;
    uint16_t RsvdZ2:15;
}__attribute__((packed))DataStageTRB;

typedef struct{
    uint32_t Reserved1;
    uint32_t Reserved2;
    uint32_t Reserved3:17;
    uint16_t Reserved4:5;
    uint16_t InterrupterTarget:10;
    uint16_t Cyclebit:1;
    uint16_t EvaluateNextTRB:1;
    uint16_t Reserved5:2;
    uint16_t Chainbit:1;
    uint16_t InterruptOnCompletion:1;
    uint16_t Reserved6:4;
    uint16_t TRBType:6;
    uint16_t Direction:1;
    uint16_t RsvdZ2:15;
}__attribute__((packed))StatusStageTRB;

typedef struct{
     uint32_t rsvrd1;
     uint32_t rsvrd2;
     uint32_t rsvrd3;
     uint8_t CycleBit:1;
     uint16_t RsvdZ1:9;
     uint16_t TRBType:6;
     uint16_t SlotType:5;
     uint16_t RsvdZ2:11;
}__attribute__((packed)) EnableSlotCommandTRB;

typedef struct{
    uint32_t Dregisters;
    uint32_t Aregisters;
    uint32_t reservedA;
    uint32_t reservedB;
    uint32_t reservedC;
    uint32_t reservedD;
    uint32_t reservedE;
    uint8_t ConfigurationValue;
    uint8_t InterfaceNumber;
    uint8_t AlternateSetting;
    uint8_t reservedF;
}__attribute__((packed)) XHCIInputControlContext;

typedef struct{
    uint32_t RouteString:20;
    uint8_t Speed:4;
    uint8_t reservedA:1;
    uint8_t MTT:1;
    uint8_t Hub:1;
    uint8_t ContextEntries:5;

    uint16_t MaxExitLatency;
    uint8_t RootHubPortNumber;
    uint8_t NumberOfPorts;

    uint8_t ParentHubSlotID;
    uint8_t ParentPortNumber;
    uint8_t TTT:2;
    uint8_t reservedB:4;
    uint16_t InterrupterTarget:10;

    uint8_t USBDeviceAddress;
    uint32_t reservedC:19;
    uint8_t SlotState:5;
}__attribute__((packed)) XHCISlotContext;

typedef struct {
    uint8_t EndpointState:3;
    uint8_t reservedA:5;
    uint8_t Mult:2;
    uint8_t MaxPStreams:5;
    uint8_t LSA:1;
    uint8_t Interval;
    uint8_t MaxESITPayloadHigh;

    uint8_t reservedB:1;
    uint8_t Cerr:2;
    uint8_t EPType:3;
    uint8_t reservedC:1;
    uint8_t HID:1;
    uint8_t MaxBurstSize;
    uint16_t MaxPacketSize;

    uint8_t DequeueCycleState:1;
    uint8_t reservedD:3;
    uint32_t TRDequeuePointerLow:28;
    uint32_t TRDequeuePointerHigh;

    uint16_t AverageTRBLength;
    uint16_t MaxESITPayloadLow; 

    uint8_t padding[0xC];
}__attribute__((packed)) XHCIEndpointContext;

typedef struct{
    XHCIInputControlContext icc;
    XHCISlotContext slotcontext;
    uint8_t paddingB[0x10];
    XHCIEndpointContext epc;
    XHCIEndpointContext epx[15];
}__attribute__((packed)) XHCIInputContextBuffer;

typedef struct{
    uint32_t DataBufferPointerLo;
    uint32_t DataBufferPointerHi;
    uint32_t rsvrd2;
    uint8_t CycleBit:1;
    uint16_t RsvdZ1:8;
    uint8_t BSR:1;
    uint16_t TRBType:6;
    uint8_t RsvdZ2;
    uint8_t SlotID;
}__attribute__((packed)) SetAddressCommandTRB;

typedef struct{
    uint32_t DataBufferPointerLo;
    uint32_t DataBufferPointerHi;
    uint32_t rsvrd2;
    uint8_t CycleBit:1;
    uint16_t RsvdZ1:8;
    uint8_t Deconfigure:1;
    uint16_t TRBType:6;
    uint8_t RsvdZ2;
    uint8_t SlotID;
}__attribute__((packed)) ConfigureEndpointCommandTRB;
