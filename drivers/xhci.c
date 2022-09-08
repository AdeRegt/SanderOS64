#include "driver.h"

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
    uint64_t address;
    uint32_t ringsegmentsize;
    uint32_t rsvd;
}__attribute__((packed)) XHCIEventRingSegmentTableEntry;

#define XHCI_OPERATIONAL_USBCMD 0x00
#define XHCI_OPERATIONAL_CONFIG 0x38
#define XHCI_RUNTIME_ERSTS 0x28
#define XHCI_RUNTIME_ERDP 0x38
#define XHCI_RUNTIME_ERSTBA 0x30
#define XHCI_OPERATIONAL_CRCR 0x18
#define XHCI_OPERATIONAL_DCBAAP 0x30
#define XHCI_OPERATIONAL_USBSTS 0x04

__attribute__((interrupt)) void xhci_int(interrupt_frame* frame){
    k_printf("XHCI: Interrupt\n");
    outportb(0x20,0x20);
}

void *xhci_bar;
XHCI_CAPABILITY_REGISTER* capabilities;
XHCIEventRingSegmentTableEntry *rts;
void *xhci_command_ring;

uint32_t xhci_read_32_register(void* where){
    return ((uint32_t*)where)[0];
}

void xhci_write_32_register(void* where,uint32_t value){
    ((uint32_t*)where)[0] = value;
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
    xhci_write_usbcmd_register(usbcmd);
}

void xhci_reset_controller(){
    uint32_t usbcmd = xhci_read_usbcmd_register();
    usbcmd &= ~2;
    xhci_write_usbcmd_register(usbcmd);
    sleep(5);
    while(xhci_read_usbcmd_register()&2);
}

void driver_start(PCIInfo *pci){
	k_printf("XHCI: Entering xhci driver....\n");
    setInterrupt(pci->inter,xhci_int);
    k_printf("XHCI: Bus: %x \n",pci->bar1);

    xhci_bar = (void*) (uint64_t) ((uint32_t)(pci->bar1 & 0xFFFFFFF0));
    capabilities = (XHCI_CAPABILITY_REGISTER*) xhci_bar;

    xhci_stop_controller();
    xhci_reset_controller();
    k_printf("xhci: Finished reset 1\n");

    // enable 16 ports
    uint32_t configres = xhci_read_config_register();
    configres |= 16;
    xhci_write_config_register(configres);
    uint32_t ersts = xhci_read_ersts_register();
    ersts &= 1;
    xhci_write_ersts_register(ersts);

    XHCIEventRingSegmentTableEntry *rts = (XHCIEventRingSegmentTableEntry*) requestPage();
    rts->ringsegmentsize = 16;
    rts->address = (uint64_t)requestPage();
    xhci_command_ring = requestPage();
    void *dcbaap = requestPage();

    memset((void*)rts->address,0,0x100);
    memset(xhci_command_ring,0,0x100);
    memset(dcbaap,0,0x100);

    xhci_write_erdp_register((uint32_t)rts->address);
    xhci_write_erstba_register((uint32_t)rts->address);
    xhci_write_crcr_register(((uint32_t)((uint64_t)xhci_command_ring))|1);
    xhci_write_dcbaap_register((uint32_t)((uint64_t)dcbaap));
    unsigned long* bse = (unsigned long*)requestPage();
    for(unsigned int i = 0 ; i < 1 ; i++){
		unsigned long spbl = (unsigned long)requestPage();
		bse[i] = spbl;
    }
    ((uint32_t*)dcbaap)[0] 	= (unsigned long)bse;
    ((uint32_t*)dcbaap)[1] 	= 0;
    xhci_start_controller();

    k_printf("Everything is still running rn....\n");
    
    sleep(100);

    for(int i = 0 ; i < 10 ; i++){
        k_printf("PORT %x \n",xhci_read_portsc_register(i));
    }

	for(;;);
}