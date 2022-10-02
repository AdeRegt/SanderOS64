#include "../../drivers/driver.h"
#include "../include/outint.h"

typedef struct {
    /**
     * @brief This field specifies the maximum number of Device
    Context Structures and Doorbell Array entries this host controller can support. Valid values are
    in the range of 1 to 255. The value of ‘0’ is reserved.
     * 
     */
    uint16_t MaxSlots:8;
    /**
     * @brief This field specifies the number of Interrupters implemented
    on this host controller. Each Interrupter may be allocated to a MSI or MSI-X vector and controls
    its generation and moderation.
    The value of this field determines how many Interrupter Register Sets are addressable in the
    Runtime Register Space (refer to section 5.5). Valid values are in the range of 1h to 400h. A ‘0’ in
    this field is undefined.
     * 
     */
    uint16_t MaxIntrs:11;
    /**
     * @brief Rsvd.
     * 
     */
    uint16_t RSVD:5;
    /**
     * @brief This field specifies the maximum Port Number value, i.e. the
    highest numbered Port Register Set that are addressable in the Operational Register Space
    (refer to Table 5-18). Valid values are in the range of 1h to FFh.
    The value in this field shall reflect the maximum Port Number value assigned by an xHCI
    Supported Protocol Capability, described in section 7.2. Software shall refer to these capabilities
    to identify whether a specific Port Number is valid, and the protocol supported by the
    associated Port Register Set.
     * 
     */
    uint16_t MaxPorts:8;
}__attribute__((packed)) XHCI_CAPABILITY_REGISTER_HCSPARAMS1;

typedef struct {
    /**
     * @brief Default = implementation dependent. The value in
    this field indicates to system software the minimum distance (in time) that it is required to stay
    ahead of the host controller while adding TRBs, in order to have the host controller process
    them at the correct time. The value shall be specified in terms of number of
    frames/microframes.
    If bit [3] of IST is cleared to '0', software can add a TRB no later than IST[2:0] Microframes
    before that TRB is scheduled to be executed.
    If bit [3] of IST is set to '1', software can add a TRB no later than IST[2:0] Frames before that TRB
    is scheduled to be executed.
    Refer to Section 4.14.2 for details on how software uses this information for scheduling
    isochronous transfers.
     * 
     */
    uint16_t IST:4;
    /**
     * @brief Default = implementation dependent. Valid values
    are 0 – 15. This field determines the maximum value supported the Event Ring Segment Table
    Base Size registers (5.5.2.3.1), where:
    The maximum number of Event Ring Segment Table entries = 2 ERST Max .
    e.g. if the ERST Max = 7, then the xHC Event Ring Segment Table(s) supports up to 128 entries,
    15 then 32K entries, etc.
     * 
     */
    uint16_t ERSTMax:4;
    /**
     * @brief Rsvd.
     * 
     */
    uint16_t Rsvd:13;
    /**
     * @brief Default = implementation dependent. This
    field indicates the high order 5 bits of the number of Scratchpad Buffers system software shall
    reserve for the xHC. Refer to section 4.20 for more information.
     * 
     */
    uint16_t MaxScratchpadBufsHi:5;
    /**
     * @brief Default = implementation dependent. If Max Scratchpad Buffers is >
    ‘0’ then this flag indicates whether the xHC uses the Scratchpad Buffers for saving state when
    executing Save and Restore State operations. If Max Scratchpad Buffers is = ‘0’ then this flag
    shall be ‘0’. Refer to section 4.23.2 for more information.
    A value of ‘1’ indicates that the xHC requires the integrity of the Scratchpad Buffer space to be
    maintained across power events.
    A value of ‘0’ indicates that the Scratchpad Buffer space may be freed and reallocated between
    power events.
     * 
     */
    uint16_t SPR:1;
    /**
     * @brief Default = implementation dependent. Valid
    values for Max Scratchpad Buffers (Hi and Lo) are 0-1023. This field indicates the low order 5
    bits of the number of Scratchpad Buffers system software shall reserve for the xHC. Refer to
    section 4.20 for more information.
     * 
     */
    uint16_t MaxScratchpadBufsLo:5;
}__attribute__((packed)) XHCI_CAPABILITY_REGISTER_HCSPARAMS2;

typedef struct {
    /**
     * @brief Worst case latency to transition a root hub Port Link State (PLS) from
    U1 to U0. Applies to all root hub ports.
     * 
     */
    uint16_t U1DeviceExitLatency:8;
    uint16_t Rsvd:8;
    /**
     * @brief Worst case latency to transition from U2 to U0. Applies to all root hub
    ports.
     * 
     */
    uint16_t U2DeviceExitLatency:16;
}__attribute__((packed)) XHCI_CAPABILITY_REGISTER_HCSPARAMS3;

typedef struct {
    /**
     * @brief This flag documents the addressing range capability of
    this implementation. The value of this flag determines whether the xHC has implemented the
    high order 32 bits of 64 bit register and data structure pointer fields. Values for this flag have the
    following interpretation:
    Value
    0
    1
    Description
    32-bit address memory pointers implemented
    64-bit address memory pointers implemented
    If 32-bit address memory pointers are implemented, the xHC shall ignore the high order 32 bits
    of 64 bit data structure pointer fields, and system software shall ignore the high order 32 bits of
    64 bit xHC registers.
     * 
     */
    uint8_t ac64:1;
    /**
     * @brief This flag identifies whether the xHC has implemented the
    Bandwidth Negotiation. Values for this flag have the following interpretation:
    Value
    0
    1
    Description
    BW Negotiation not implemented
    BW Negotiation implemented
    Refer to section 4.16 for more information on Bandwidth Negotiation.
     * 
     */
    uint8_t bnc:1;
    /**
     * @brief If this bit is set to ‘1’, then the xHC uses 64 byte Context data structures. If
    this bit is cleared to ‘0’, then the xHC uses 32 byte Context data structures.
    Note: This flag does not apply to Stream Contexts.
     * 
     */
    uint8_t csz:1;
    /**
     * @brief This flag indicates whether the host controller implementation
    includes port power control. A ‘1’ in this bit indicates the ports have port power switches. A ‘0’ in
    this bit indicates the port do not have port power switches. The value of this flag affects the
    functionality of the PP flag in each port status and control register (refer to Section 5.4.8).
     * 
     */
    uint8_t ppc:1;
    /**
     * @brief This bit indicates whether the xHC root hub ports support port indicator
    control. When this bit is a ‘1’, the port status and control registers include a read/writeable field
    for controlling the state of the port indicator. Refer to Section 5.4.8 for definition of the Port
    Indicator Control field.
     * 
     */
    uint8_t pind:1;
    /**
     * @brief This flag indicates whether the host controller implementation
    supports a Light Host Controller Reset. A ‘1’ in this bit indicates that Light Host Controller Reset is
    supported. A ‘0’ in this bit indicates that Light Host Controller Reset is not supported. The value
    of this flag affects the functionality of the Light Host Controller Reset (LHCRST) flag in the
    USBCMD register (refer to Section 5.4.1).
     * 
     */
    uint8_t lhrc:1;
    /**
     * @brief This flag indicates whether the host controller
    implementation supports Latency Tolerance Messaging (LTM). A ‘1’ in this bit indicates that LTM
    is supported. A ‘0’ in this bit indicates that LTM is not supported. Refer to section 4.13.1 for more
    information on LTM.
     * 
     */
    uint8_t ltc:1;
    /**
     * @brief This flag indicates whether the host controller
    implementation supports Secondary Stream IDs. A ‘1’ in this bit indicates that Secondary Stream
    ID decoding is not supported. A ‘0’ in this bit indicates that Secondary Stream ID decoding is
    supported. (refer to Sections 4.12.2 and 6.2.3).
     * 
     */
    uint8_t nss:1;
    /**
     * @brief This flag indicates whether the host controller implementation
    Parses all Event Data TRBs while advancing to the next TD after a Short Packet, or it skips all but
    the first Event Data TRB. A ‘1’ in this bit indicates that all Event Data TRBs are parsed. A ‘0’ in this
    bit indicates that only the first Event Data TRB is parsed (refer to section 4.10.1.1).
     * 
     */
    uint8_t pae:1;
    /**
     * @brief This flag indicates that the host controller
    implementation is capable of generating a Stopped - Short Packet Completion Code. Refer to
    section 4.6.9 for more information.
     * 
     */
    uint8_t spc:1;
    /**
     * @brief This flag indicates that the host controller implementation
    Stream Context support a Stopped EDTLA field. Refer to sections 4.6.9, 4.12, and 6.4.4.1 for more
    information.
    Stopped EDTLA Capability support (i.e. SEC = '1') shall be mandatory for all xHCI 1.1 and xHCI 1.2
    compliant xHCs.
     * 
     */
    uint8_t sec:1;
    /**
     * @brief This flag indicates that the host controller
    implementation is capable of matching the Frame ID of consecutive Isoch TDs. Refer to section
    4.11.2.5 for more information.
     * 
     */
    uint8_t cfc:1;
    /**
     * @brief This fields identifies the maximum size
    Primary Stream Array that the xHC supports. The Primary Stream Array size = 2 MaxPSASize+1 . Valid
    MaxPSASize values are 0 to 15, where ‘0’ indicates that Streams are not supported.
     * 
     */
    uint8_t maxpsasize:4;
    /**
     * @brief This field indicates the existence of a capabilities list.
    The value of this field indicates a relative offset, in 32-bit words, from Base to the beginning of
    the first extended capability.
    For example, using the offset of Base is 1000h and the xECP value of 0068h, we can calculated
    the following effective address of the first extended capability:
    1000h + (0068h << 2) -> 1000h + 01A0h -> 11A0h
     * 
     */
    uint16_t xecp:15;
}__attribute__((packed)) XHCI_CAPABILITY_REGISTER_HCCPARAMS1;

typedef struct{
    uint8_t U3C:1;
    uint8_t CMC:1;
    uint8_t FSC:1;
    uint8_t CTC:1;
    uint8_t LEC:1;
    uint8_t CIC:1;
    uint8_t ETC:1;
    uint8_t ETC_TSC:1;
    uint8_t GSC:1;
    uint8_t VTC:1;
    uint32_t res:22;
}__attribute__((packed)) XHCI_CAPABILITY_REGISTER_HCCPARAMS2;

typedef struct{
    /**
     * @brief This register is used as an offset to add to register base to find the beginning of
    the Operational Register Space.
     * 
     */
    uint8_t CAPLENGTH;
    /**
     * @brief reserved
     * 
     */
    uint8_t RSVD;
    /**
     * @brief This is a two-byte register containing a BCD encoding of the xHCI specification
    revision number supported by this host controller. The most significant byte of
    this register represents a major revision and the least significant byte contains
    the minor revision extensions. e.g. 0100h corresponds to xHCI version 1.0.0, or
    0110h corresponds to xHCI version 1.1.0, etc.
     * 
     */
    uint16_t HCIVERSION;
    /**
     * @brief This register defines basic structural parameters supported by this xHC
    implementation: Number of Device Slots support, Interrupters, Root Hub ports,
    etc.
     * 
     */
    XHCI_CAPABILITY_REGISTER_HCSPARAMS1 HCSPARAMS1;
    XHCI_CAPABILITY_REGISTER_HCSPARAMS2 HCSPARAMS2;
    XHCI_CAPABILITY_REGISTER_HCSPARAMS3 HCSPARAMS3;
    XHCI_CAPABILITY_REGISTER_HCCPARAMS1 HCCPARAMS1;
    uint32_t DBOFF;
    uint32_t RTSOFF;
    XHCI_CAPABILITY_REGISTER_HCCPARAMS2 HCCPARAMS2;
}__attribute__((packed)) XHCI_CAPABILITY_REGISTER;

typedef struct{
    upointer_t addresslow;
    #ifndef __x86_64
    upointer_t addresshigh;
    #endif
    uint32_t ringsegmentsize;
    uint32_t rsvd;
}__attribute__((packed)) XHCIEventRingSegmentTableEntry;

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
    #ifdef __x86_64
    upointer_t TRDequeuePointer:59;
    #endif 
    #ifndef __x86_64
    upointer_t TRDequeuePointerLow:28;
    upointer_t TRDequeuePointerHigh;
    #endif

    uint16_t AverageTRBLength;
    uint16_t MaxESITPayloadLow; 
}__attribute__((packed)) XHCIEndpointContext;

typedef struct{
    XHCIInputControlContext icc;
    uint8_t paddingA[32];
    XHCISlotContext slotcontext;
    // uint8_t paddingB[0x10];
    uint8_t paddingB[0x10 + 0x20];
    XHCIEndpointContext epc;
    XHCIEndpointContext epx[15];
}__attribute__((packed)) XHCIInputContextBuffer;

typedef struct{
    #ifdef __x86_64
    upointer_t DataBufferPointerHiandLo;
    #endif 
    #ifndef __x86_64
    upointer_t DataBufferPointerLo;
    upointer_t DataBufferPointerHi;
    #endif 
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
     uint32_t reserved1:24;
     uint16_t PortID:8;
     uint32_t reserved2:24;
     uint16_t CompletionCode:8;
     uint8_t C:1;
     uint16_t reserved3:9;
     uint8_t TRBType:6;
     uint16_t reserved4;
}__attribute__((packed)) PortStatusChangeEventTRB;

typedef struct{
     uint32_t rsvrd1;
     uint32_t rsvrd2;
     uint32_t rsvrd3;
     uint8_t CycleBit:1;
     uint16_t RsvdZ1:9;
     uint16_t TRBType:6;
     uint16_t RsvdZ2:16;
}__attribute__((packed)) NoOperationCommandTRB;

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
     uint64_t InputContextPointer;
     uint32_t rsvrd2;
     uint8_t CycleBit:1;
     uint16_t RsvdZ1:8;
     uint8_t BSR:1;
     uint16_t TRBType:6;
     uint8_t RsvdZ2;
     uint8_t SlotID;
}__attribute__((packed)) SetAddressCommandTRB;

typedef struct{
     uint64_t address;
     uint32_t CommandCompletionParameter:24;
     uint16_t CompletionCode:8;
     uint8_t C:1;
     uint16_t reserved3:9;
     uint8_t TRBType:6;
     uint32_t VFID:8;
     uint32_t SlotID:8;
}__attribute__((packed)) CommandCompletionEventTRB;

#define XHCI_OPERATIONAL_USBCMD 0x00
#define XHCI_OPERATIONAL_USBSTS 0x04
#define XHCI_OPERATIONAL_DNCTRL 0x14
#define XHCI_OPERATIONAL_CRCR 0x18
#define XHCI_OPERATIONAL_DCBAAP 0x30
#define XHCI_OPERATIONAL_CONFIG 0x38
#define XHCI_RUNTIME_ERSTS 0x28
#define XHCI_RUNTIME_ERDP 0x38
#define XHCI_RUNTIME_ERSTBA 0x30

#define XHCI_SIZE_EVENT_RING 16
#define XHCI_SIZE_TRB 8

void *xhci_bar;
XHCI_CAPABILITY_REGISTER* capabilities;
XHCIEventRingSegmentTableEntry *rts;
void *xhci_command_ring;
void *xhci_event_ring;
int xhci_command_ring_pointer = 0;

uint32_t xhci_read_32_register(void* where){
    return ((uint32_t*)where)[0];
}

void xhci_write_32_register(void* where,uint32_t value){
    ((uint32_t*)where)[0] = value;
}

uint32_t xhci_read_dnctrl_register(){
    return xhci_read_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_DNCTRL);
}

void xhci_write_dnctrl_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_DNCTRL,value);
}

uint32_t xhci_read_usbcmd_register(){
    return xhci_read_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_USBCMD);
}

void xhci_write_usbcmd_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_USBCMD,value);
}

uint32_t xhci_read_config_register(){
    return xhci_read_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_CONFIG);
}

void xhci_write_config_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_CONFIG,value);
}

uint32_t xhci_read_ersts_register(){
    return xhci_read_32_register(xhci_bar + capabilities->RTSOFF + XHCI_RUNTIME_ERSTS);
}

void xhci_write_ersts_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->RTSOFF + XHCI_RUNTIME_ERSTS,value);
}

uint32_t xhci_read_erdp_register(){
    return xhci_read_32_register(xhci_bar + capabilities->RTSOFF + XHCI_RUNTIME_ERDP);
}

void xhci_write_erdp_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->RTSOFF + XHCI_RUNTIME_ERDP,value);
    xhci_write_32_register(xhci_bar + capabilities->RTSOFF + XHCI_RUNTIME_ERDP + sizeof(uint32_t),0);
}

uint32_t xhci_read_erstba_register(){
    return xhci_read_32_register(xhci_bar + capabilities->RTSOFF + XHCI_RUNTIME_ERSTBA);
}

void xhci_write_erstba_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->RTSOFF + XHCI_RUNTIME_ERSTBA,value);
    xhci_write_32_register(xhci_bar + capabilities->RTSOFF + XHCI_RUNTIME_ERSTBA + sizeof(uint32_t),0);
}

uint32_t xhci_read_crcr_register(){
    return xhci_read_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_CRCR);
}

void xhci_write_crcr_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_CRCR,value);
    xhci_write_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_CRCR + sizeof(uint32_t),0);
}

uint32_t xhci_read_dcbaap_register(){
    return xhci_read_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_DCBAAP);
}

void xhci_write_dcbaap_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_DCBAAP,value);
}

uint32_t xhci_read_usbsts_register(){
    return xhci_read_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_USBSTS);
}

void xhci_write_usbsts_register(uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->CAPLENGTH + XHCI_OPERATIONAL_USBSTS,value);
}

uint32_t xhci_read_portsc_register(int port){
    return xhci_read_32_register(xhci_bar + capabilities->CAPLENGTH + (0x400 + (0x10 * (port-1))));
}

void xhci_write_portsc_register(int port,uint32_t value){
    xhci_write_32_register(xhci_bar + capabilities->CAPLENGTH + (0x400 + (0x10 * (port-1))),value);
}

uint8_t xhci_usbcmd_is_running(){
    return xhci_read_usbcmd_register() & 1;
}

void xhci_stop_controller(){
    if(xhci_usbcmd_is_running()){
        uint32_t usbcmd = xhci_read_usbcmd_register();
        usbcmd &= ~1;
        xhci_write_usbcmd_register(usbcmd);
    }
}

void xhci_start_controller(){
    uint32_t usbcmd = xhci_read_usbcmd_register();
    usbcmd |= 1;
    k_printf("xhci: starting the controller...\n");
    xhci_write_usbcmd_register(usbcmd);
}

void xhci_reset_controller(){
    uint32_t usbcmd = xhci_read_usbcmd_register();
    usbcmd &= ~2;
    xhci_write_usbcmd_register(usbcmd);
    sleep(5);
    while(xhci_read_usbcmd_register()&2);
}

__attribute__((interrupt)) void xhci_int(interrupt_frame* frame){
    k_printf("xhci: Interrupt\n");

	outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

void xhci_ring_doorbell(int index,int value){
    ((uint32_t*)(xhci_bar + capabilities->DBOFF))[index] = value;
}

void *xhci_ring_and_wait(void* trb,int index,int value){
    k_printf("xhci: Waiting for response for %x ",trb);
    xhci_ring_doorbell(index,value);
    k_printf(":");
    sleep(20);
    int timeout = 20;
    while(1){
        k_printf(".");
        uint32_t* tv = (uint32_t*) xhci_event_ring;
        for(int i = 0 ; i < XHCI_SIZE_EVENT_RING*4 ; i++){
            if(tv[i]==(uint32_t)((uint64_t) trb)){
                k_printf("\n");
                return (void*)&tv[i];
            }
        }
        sleep(10);
        timeout--;
        if(timeout==0){
            break;
        }
    }
    k_printf("FAILED: timeout\n");
    // for(int i = 0 ; i < 10 ; i++){k_printf("%x %x %x %x | ",((uint32_t*)xhci_event_ring)[(i*4)+0],((uint32_t*)xhci_event_ring)[(i*4)+1],((uint32_t*)xhci_event_ring)[(i*4)+2],((uint32_t*)xhci_event_ring)[(i*4)+3]);}for(;;);
    return 0;
}

void* xhci_get_next_free_command(){
    void *res = (xhci_command_ring + (xhci_command_ring_pointer*XHCI_SIZE_TRB));
    xhci_command_ring_pointer++;
    xhci_command_ring_pointer++;
    return res;
}

int xhci_noop_command_ring(){
    NoOperationCommandTRB *noopcmd = (NoOperationCommandTRB*) xhci_get_next_free_command();
    noopcmd->CycleBit = 1;
    noopcmd->TRBType = 23;

    CommandCompletionEventTRB *result = xhci_ring_and_wait(noopcmd,0,0);
    if(result){
        return result->CompletionCode;
    }else{
        return 0;
    }
}

int xhci_request_device_id(){
    EnableSlotCommandTRB *noopcmd = (EnableSlotCommandTRB*) xhci_get_next_free_command();
    noopcmd->CycleBit = 1;
    noopcmd->TRBType = 9;
    // noopcmd->SlotType = 3;

    CommandCompletionEventTRB *result = xhci_ring_and_wait(noopcmd,0,0);
    if(result){
        if(result->CompletionCode==1){
            return result->SlotID;
        }else{
            k_printf("xhci: unexpected completion code: %d \n",result->CompletionCode);
            return -2;
        }
    }else{
        k_printf("xhci: cannot find type\n");
        return -1;
    }
}

int xhci_set_address(void* pointer,uint8_t slotid){
    SetAddressCommandTRB *noopcmd = (SetAddressCommandTRB*) xhci_get_next_free_command();
    noopcmd->CycleBit = 1;
    noopcmd->TRBType = 11;
    noopcmd->InputContextPointer = (uint64_t)pointer;
    noopcmd->SlotID = slotid;
    noopcmd->BSR = 0;

    CommandCompletionEventTRB *result = xhci_ring_and_wait(noopcmd,0,0);
    return result->CompletionCode;
}

char* xhci_get_portspeed_string(uint8_t portspeed){
    if(portspeed==1){
        return "full-speed 12mb/s usb2.0";
    }
    if(portspeed==2){
        return "low-speed 1.5mb/s usb2.0";
    }
    if(portspeed==3){
        return "high-speed 480mb/s usb2.0";
    }
    if(portspeed==4){
        return "super-speed 6gb/s usb3.0";
    }
    return "Illegal value";
}

char* xhci_get_status_code_string(int statuscode){
    if(statuscode==0){
        return "invalid";
    }
    if(statuscode==1){
        return "Success";
    }
    if(statuscode==2){
        return "Data Buffer Error";
    }
    if(statuscode==3){
        return "Babble Detected Error";
    }
    if(statuscode==4){
        return "USB Transaction Error";
    }
    if(statuscode==5){
        return "TRB Error";
    }
    if(statuscode==6){
        return "Stall Error";
    }
    if(statuscode==7){
        return "Resource Error";
    }
    if(statuscode==8){
        return "Bandwidth Error";
    }
    if(statuscode==9){
        return "No Slots Available Error";
    }
    if(statuscode==10){
        return "Invalid Stream Type Error";
    }
    if(statuscode==11){
        return "Slot Not Enabled Error";
    }
    if(statuscode==12){
        return "Endpoint Not Enabled Error";
    }
    if(statuscode==13){
        return "Short Packet";
    }
    if(statuscode==14){
        return "Ring Underrun";
    }
    if(statuscode==15){
        return "Ring Overrun";
    }
    if(statuscode==16){
        return "VF Event Ring Full Error";
    }
    if(statuscode==17){
        return "Parameter Error";
    }
    if(statuscode==18){
        return "Bandwidth Overrun Error";
    }
    if(statuscode==19){
        return "Context State Error";
    }
    if(statuscode==20){
        return "No Ping Response Error";
    }
    if(statuscode==21){
        return "Event Ring Full Error";
    }
    if(statuscode==22){
        return "Incompatible Device Error";
    }
    if(statuscode==23){
        return "Missed Service Error";
    }
    if(statuscode==24){
        return "Command Ring Stopped";
    }
    if(statuscode==25){
        return "Command Aborted";
    }
    if(statuscode==26){
        return "Stopped";
    }
    if(statuscode==27){
        return "Stopped - Length Invalid";
    }
    if(statuscode==28){
        return "Stopped - Short Packet";
    }
    if(statuscode==29){
        return "Max Exit Latency Too Large Error";
    }
    return "unknown status code";
}

void xhci_driver_start(int bus,int slot,int function){
    int inter = getBARaddress(bus,slot,function,0x3C) & 0x000000FF;
    uint32_t bar1 = getBARaddress(bus,slot,function,0x10);
	k_printf("xhci: Entering xhci driver....\n");
    setInterrupt(inter,xhci_int);

    xhci_bar = (void*) (uint64_t) ((uint32_t)(bar1 & 0xFFFFFFF0));
    capabilities = (XHCI_CAPABILITY_REGISTER*) xhci_bar;

    k_printf("xhci: Bus: %x \n",xhci_bar);
    if(xhci_bar==0){
        k_printf("xhci: seems there is nothing here for us....\n");
        return;
    }
    k_printf("xhci: status before reset.... USBCMD: %x USBSTS: %x DNCNTRL: %x \n",xhci_read_usbcmd_register(),xhci_read_usbsts_register(),xhci_read_dnctrl_register());

    uint32_t olddcbaapvalue = xhci_read_dcbaap_register();

    xhci_stop_controller();
    xhci_reset_controller();
    k_printf("xhci: Finished reset, according to the system, we have %d ports available\n",capabilities->HCSPARAMS1.MaxPorts);
    xhci_write_usbcmd_register(0);

    uint32_t configres = xhci_read_config_register();
    configres |= capabilities->HCSPARAMS1.MaxPorts;
    xhci_write_config_register(configres);

    xhci_write_ersts_register(10);

    xhci_event_ring = requestPage();

    rts = (XHCIEventRingSegmentTableEntry*) requestPage();
    rts->ringsegmentsize = XHCI_SIZE_EVENT_RING;
    rts->addresslow = (uint64_t) xhci_event_ring;
    xhci_command_ring = requestPage();

    memset((void*)xhci_event_ring,0,0x100);
    memset(xhci_command_ring,0,0x100);

    xhci_write_erdp_register((uint32_t)((uint64_t)xhci_event_ring));
    xhci_write_erstba_register((uint32_t)((uint64_t)rts));
    xhci_write_crcr_register(((uint32_t)((uint64_t)xhci_command_ring)) | 1);

    xhci_write_dnctrl_register(0xFFFF);

    void *dcbaap = (void*) requestPage();
    memset(dcbaap,0,0xa00);
    for(int i = 0 ; i < sizeof(uint64_t) ; i++){
        uint8_t ov = ((uint8_t*)((uint64_t)olddcbaapvalue))[i];
        ((uint8_t*)dcbaap)[i] = ov;
    }
    xhci_write_dcbaap_register((uint32_t)((uint64_t)dcbaap));

    xhci_start_controller();

    int noop = xhci_noop_command_ring();
    if(noop==1){
        k_printf("xhci: NOOP succeed with 1 !\n");
        for(int portnumber = 1 ; portnumber < capabilities->HCSPARAMS1.MaxPorts ; portnumber++){
            uint32_t initial_portsc_status = xhci_read_portsc_register(portnumber);
            uint8_t portspeed = (initial_portsc_status>>10) & 0xF;
            if( (initial_portsc_status&3)!=0 && (initial_portsc_status&0x200)==0x200 ){
                xhci_write_portsc_register(portnumber,initial_portsc_status | 0x10 );
                sleep(20);
                initial_portsc_status = xhci_read_portsc_register(portnumber);
                portspeed = (initial_portsc_status>>10) & 0xF;
            }
            if((initial_portsc_status&3)==3){
                k_printf("xhci-%d: This port deserves our attention and it has the status of %x and a portspeed of %s \n",portnumber,initial_portsc_status,xhci_get_portspeed_string(portspeed));
                if((initial_portsc_status>>5)&0xF!=0){
                    continue;
                }

                uint16_t MaxPackageSize = 0;
                if(portspeed==4){
                    MaxPackageSize = 512;
                }else if(portspeed==1||portspeed==3){
                    MaxPackageSize = 64;
                }else{
                    MaxPackageSize = 8;
                }

                // first we need a device-id
                int device_id = xhci_request_device_id();
                if(device_id<1){
                    k_printf("xhci-%d: Unable to get a device-id!\n",portnumber);
                    continue;
                }
                k_printf("xhci-%d: This port recieved a device id of %d \n",portnumber,device_id);

                void* local_ring = requestPage();

                // after this, we need an address
                XHCIInputContextBuffer *ic = (XHCIInputContextBuffer*) requestPage();
                memset(ic,0,sizeof(XHCIInputContextBuffer));
                ic->icc.Aregisters = 3;
                k_printf("dbg: icc=%x sc=%x epc=%x \n",(uint64_t)&ic->icc,(uint64_t)&ic->slotcontext,(uint64_t)&ic->epc);

                ic->slotcontext.RootHubPortNumber = portnumber;
                ic->slotcontext.ContextEntries = 1;
                ic->slotcontext.Speed = portspeed;

                ic->epc.EndpointState = 0;
                ic->epc.LSA = 1;
                ic->epc.EPType = 4;
                ic->epc.Cerr = 3;
                ic->epc.MaxPacketSize = MaxPackageSize;
                #ifdef __x86_64
                    ic->epc.TRDequeuePointer = ((uint64_t)local_ring)>>4;
                #endif
                #ifndef __x86_64
                    ic->epc.TRDequeuePointerLow = ((uint64_t)local_ring)>>4;
                #endif 
                ic->epc.DequeueCycleState = 1;
                ic->epc.AverageTRBLength = 8;

                XHCISlotContext *outputcontext = (XHCISlotContext*) requestPage();
                memset(outputcontext,0,sizeof(XHCISlotContext)+0x40);
                ((uint64_t*)dcbaap)[device_id] = (uint64_t)outputcontext;

                int sacc = xhci_set_address(ic,device_id);
                k_printf("xhci: setaddress completion code: %s \n",xhci_get_status_code_string(sacc));
            }
        }
    }else{
        k_printf("xhci: NOOP failed with %d !\n",noop);
    }

    k_printf("Thatsit\n");
    for(;;);
}