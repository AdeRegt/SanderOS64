#pragma once
#include <stdint.h>

#define XHCI_TRB_TYPE_EVENT_TRANSFER_EVENT 32
#define XHCI_TRB_TYPE_EVENT_COMMAND_COMPLETION_EVENT 33
#define XHCI_TRB_TYPE_EVENT_PORT_STATUS_CHANGE_EVENT 34
#define XHCI_TRB_TYPE_EVENT_BANDWIDTH_REQUEST_EVENT 35
#define XHCI_TRB_TYPE_EVENT_DOORBELL_EVENT 36
#define XHCI_TRB_TYPE_EVENT_HOST_CONTROLLER_EVENT 37
#define XHCI_TRB_TYPE_EVENT_DEVICE_NOTIFICATION_EVENT 38
#define XHCI_TRB_TYPE_EVENT_MFINDEX_WRAP_EVENT 39

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
    /**
 * @brief Default = ‘0’. ‘1’ = Run. ‘0’ = Stop. When set to a ‘1’, the xHC proceeds with
execution of the schedule. The xHC continues execution as long as this bit is set to a ‘1’. When
this bit is cleared to ‘0’, the xHC completes any current or queued commands or TDs, and any
USB transactions associated with them, then halts.
Refer to section 5.4.1.1 for more information on how R/S shall be managed.
The xHC shall halt within 16 ms. after software clears the Run/Stop bit if the above conditions
have been met.
The HCHalted (HCH) bit in the USBSTS register indicates when the xHC has finished its pending
pipelined transactions and has entered the stopped state. Software shall not write a ‘1’ to this
flag unless the xHC is in the Halted state (i.e. HCH in the USBSTS register is ‘1’). Doing so may
yield undefined results. Writing a ‘0’ to this flag when the xHC is in the Running state (i.e. HCH =
‘0’) and any Event Rings are in the Event Ring Full state (refer to section 4.9.4) may result in lost
events.
 * 
 */
    uint8_t run_stop:1;
    /**
     * @brief Default = ‘0’. This control bit is used by software to reset
the host controller. The effects of this bit on the xHC and the Root Hub registers are similar to a
Chip Hardware Reset.
When software writes a ‘1’ to this bit, the Host Controller resets its internal pipelines, timers,
counters, state machines, etc. to their initial value. Any transaction currently in progress on the
USB is immediately terminated. A USB reset shall not be driven on USB2 downstream ports,
however a Hot or Warm Reset 79 shall be initiated on USB3 Root Hub downstream ports.
PCI Configuration registers are not affected by this reset. All operational registers, including port
registers and port state machines are set to their initial values. Software shall reinitialize the
host controller as described in Section 4.2 in order to return the host controller to an
operational state.
This bit is cleared to ‘0’ by the Host Controller when the reset process is complete. Software
cannot terminate the reset process early by writing a ‘0’ to this bit and shall not write any xHC
Operational or Runtime registers until while HCRST is ‘1’. Note, the completion of the xHC reset
process is not gated by the Root Hub port reset process.
Software shall not set this bit to ‘1’ when the HCHalted (HCH) bit in the USBSTS register is a ‘0’.
Attempting to reset an actively running host controller may result in undefined behavior.
When this register is exposed by a Virtual Function (VF), this bit only resets the xHC instance
presented by the selected VF. Refer to section 8 for more information.
     * 
     */
    uint8_t hcrst:1;
    /**
     * @brief Default = ‘0’. This bit provides system software with a means of
enabling or disabling the host system interrupts generated by Interrupters. When this bit is a ‘1’,
then Interrupter host system interrupt generation is allowed, e.g. the xHC shall issue an interrupt
at the next interrupt threshold if the host system interrupt mechanism (e.g. MSI, MSI-X, etc.) is
enabled. The interrupt is acknowledged by a host system interrupt specific mechanism.
When this register is exposed by a Virtual Function (VF), this bit only enables the set of
Interrupters assigned to the selected VF. Refer to section 7.7.2 for more information.
     * 
     */
    uint8_t inte:1;
    /**
     * @brief Default = ‘0’. When this bit is a ‘1’, and the HSE bit in
the USBSTS register is a ‘1’, the xHC shall assert out-of-band error signaling to the host. The
signaling is acknowledged by software clearing the HSE bit. Refer to section 4.10.2.6 for more
information.
When this register is exposed by a Virtual Function (VF), the effect of the assertion of this bit on
the Physical Function (PF0) is determined by the VMM. Refer to section 8 for more information.
     * 
     */
    uint8_t hsee:1;
    /**
     * @brief RsvdP.
     * 
     */
    uint8_t rsvd1:3;
    /**
     * @brief Optional normative. Default = ‘0’. If the Light
HC Reset Capability (LHRC) bit in the HCCPARAMS1 register is ‘1’, then this flag allows the driver
to reset the xHC without affecting the state of the ports.
A system software read of this bit as ‘0’ indicates the Light Host Controller Reset has completed
and it is safe for software to re-initialize the xHC. A software read of this bit as a ‘1’ indicates the
Light Host Controller Reset has not yet completed.
If not implemented, a read of this flag shall always return a ‘0’.
All registers in the Aux Power well shall maintain the values that had been asserted prior to the
Light Host Controller Reset. Refer to section 4.23.1 for more information.
When this register is exposed by a Virtual Function (VF), this bit only generates a Light Reset to
the xHC instance presented by the selected VF, e.g. Disable the VFs’ device slots and set the
associated VF Run bit to Stopped. Refer to section 8 for more information.
     * 
     */
    uint8_t lhcrst:1;
    /**
     * @brief Default = ‘0’. When written by software with ‘1’ and HCHalted
(HCH) = ‘1’, then the xHC shall save any internal state (that may be restored by a subsequent
Restore State operation) and if FSC = '1' any cached Slot, Endpoint, Stream, or other Context
information (so that software may save it). When written by software with ‘1’ and HCHalted
(HCH) = ‘0’, or written with ‘0’, no Save State operation shall be performed. This flag always
returns ‘0’ when read. Refer to the Save State Status (SSS) flag in the USBSTS register for
information on Save State completion. Refer to section 4.23.2 for more information on xHC
Save/Restore operation. Note that undefined behavior may occur if a Save State operation is
initiated while Restore State Status (RSS) = ‘1’.
When this register is exposed by a Virtual Function (VF), this bit only controls saving the state of
the xHC instance presented by the selected VF. Refer to section 8 for more information.
     * 
     */
    uint8_t css:1;
    /**
     * @brief Default = ‘0’. When set to ‘1’, and HCHalted (HCH) = ‘1’,
then the xHC shall perform a Restore State operation and restore its internal state. When set to
‘1’ and Run/Stop (R/S) = ‘1’ or HCHalted (HCH) = ‘0’, or when cleared to ‘0’, no Restore State
operation shall be performed. This flag always returns ‘0’ when read. Refer to the Restore State
Status (RSS) flag in the USBSTS register for information on Restore State completion. Refer to
section 4.23.2 for more information. Note that undefined behavior may occur if a Restore State
operation is initiated while Save State Status (SSS) = ‘1’.
When this register is exposed by a Virtual Function (VF), this bit only controls restoring the state
of the xHC instance presented by the selected VF. Refer to section 8 for more information.
     * 
     */
    uint8_t crs:1;
    /**
     * @brief Default = ‘0’. When set to ‘1’, the xHC shall generate a MFINDEX
Wrap Event every time the MFINDEX register transitions from 03FFFh to 0. When cleared to ‘0’
no MFINDEX Wrap Events are generated. Refer to section 4.14.2 for more information.
When this register is exposed by a Virtual Function (VF), the generation of MFINDEX Wrap
Events to VFs shall be emulated by the VMM.
     * 
     */
    uint8_t ewe:1;
    /**
     * @brief Default = ‘0’. When set to ‘1’, the xHC may stop the
MFINDEX counting action if all Root Hub ports are in the U3, Disconnected, Disabled, or
Powered-off state. When cleared to ‘0’ the xHC may stop the MFINDEX counting action if all
Root Hub ports are in the Disconnected, Disabled, Training, or Powered-off state. Refer to
section 4.14.2 for more information.
     * 
     */
    uint8_t eu3s:1;
    uint8_t rsvd2:1;
    /**
     * @brief Default = '0'. When set to '1', a Max Exit Latency Too Large Capability
Error may be returned by a Configure Endpoint Command. When cleared to '0', a Max Exit
Latency Too Large Capability Error shall not be returned by a Configure Endpoint Command.
This bit is Reserved if CMC = ‘0’. Refer to section 4.23.5.2.2 for more information.
     * 
     */
    uint8_t cme:1;
    /**
     * @brief This flag indicates that the host controller implementation is
enabled to support Transfer Burst Count (TBC) values greater that 4 in isoch TDs. When this bit
is ‘1’, the Isoch TRB TD Size/TBC field presents the TBC value, and the TBC/RsvdZ field is RsvdZ.
When this bit is ‘0’, the TDSize/TCB field presents the TD Size value, and the TBC/RsvdZ field
presents the TBC value. This bit may be set only if ETC = ‘1’. Refer to section 4.11.2.3 for more
information.
     * 
     */
    uint8_t ete:1;
    /**
     * @brief This flag indicates that the host controller
implementation is enabled to support ETC_TSC capability. When this is ‘1’, TRBSts field in the
TRB updated to indicate if it is last transfer TRB in the TD. This bit may be set only if
ETC_TSC=’1’. Refer to section 4.11.2.3 for more information.
     * 
     */
    uint8_t tsc_en:1;
    /**
     * @brief Default = ‘0’. When set to ‘1’, XHCI HW will enable its VTIO
capability and begin to use the information provided via that VTIO Registers to determine its
DMA-ID. When cleared to ‘0’, XHCI HW will use the Primary DMA-ID for all accesses. This bit
may be set only if VTC = ‘1’.
     * 
     */
    uint8_t vtioe:1;
    uint16_t rsvd3:15;
}__attribute__((packed)) XHCI_OPERATIONAL_REGISTER_USBCMD;

typedef struct{
    /**
     * @brief Default = ‘1’. This bit is a ‘0’ whenever the Run/Stop (R/S) bit is a ‘1’. The
xHC sets this bit to ‘1’ after it has stopped executing as a result of the Run/Stop (R/S) bit being
cleared to ‘0’, either by software or by the xHC hardware (e.g. internal error).
If this bit is '1', then SOFs, microSOFs, or Isochronous Timestamp Packets (ITP) shall not be
generated by the xHC, and any received Transaction Packet shall be dropped.
When this register is exposed by a Virtual Function (VF), this bit only reflects the Halted state of
the xHC instance presented by the selected VF. Refer to section 8 for more information.
     * 
     */
    uint8_t hch:1;
    uint8_t rsvd1:1;
    /**
     * @brief Default = ‘0’. The xHC sets this bit to ‘1’ when a serious error
is detected, either internal to the xHC or during a host system access involving the xHC module.
(In a PCI system, conditions that set this bit to ‘1’ include PCI Parity error, PCI Master Abort, and
PCI Target Abort.) When this error occurs, the xHC clears the Run/Stop (R/S) bit in the USBCMD
register to prevent further execution of the scheduled TDs. If the HSEE bit in the USBCMD
register is a ‘1’, the xHC shall also assert out-of-band error signaling to the host. Refer to section
4.10.2.6 for more information.
When this register is exposed by a Virtual Function (VF), the assertion of this bit affects all VFs
and reflects the Host System Error state of the Physical Function (PF0). Refer to section 8 for
more information.
     * 
     */
    uint8_t hse:1;
    /**
     * @brief Default = ‘0’. The xHC sets this bit to ‘1’ when the Interrupt
Pending (IP) bit of any Interrupter transitions from ‘0’ to ‘1’. Refer to section 7.1.2 for use.
Software that uses EINT shall clear it prior to clearing any IP flags. A race condition may occur if
software clears the IP flags then clears the EINT flag, and between the operations another IP ‘0’
to '1' transition occurs. In this case the new IP transition shall be lost.
When this register is exposed by a Virtual Function (VF), this bit is the logical 'OR' of the IP bits
for the Interrupters assigned to the selected VF. And it shall be cleared to ‘0’ when all associated
interrupter IP bits are cleared, i.e. all the VF’s Interrupter Event Ring(s) are empty. Refer to
section 8 for more information.
     * 
     */
    uint8_t eint:1;
    /**
     * @brief Default = ‘0’. The xHC sets this bit to a ‘1’ when any port has
a change bit transition from a ‘0’ to a ‘1’.
This bit is allowed to be maintained in the Aux Power well. Alternatively, it is also acceptable
that on a D3 to D0 transition of the xHC, this bit is loaded with the OR of all of the PORTSC
change bits. Refer to section 4.19.3.
This bit provides system software an efficient means of determining if there has been Root Hub
port activity. Refer to section 4.15.2.3 for more information.
When this register is exposed by a Virtual Function (VF), the VMM determines the state of this
bit as a function of the Root Hub Ports associated with the Device Slots assigned to the selected
VF. Refer to section 8 for more information.
     * 
     */
    uint8_t pcd:1;
    uint8_t rsvd2:3;
    /**
     * @brief Default = ‘0’. When the Controller Save State (CSS) flag in the
USBCMD register is written with ‘1’ this bit shall be set to ‘1’ and remain 1 while the xHC saves
its internal state. When the Save State operation is complete, this bit shall be cleared to ‘0’.
Refer to section 4.23.2 for more information.
When this register is exposed by a Virtual Function (VF), the VMM determines the state of this
bit as a function of the saving the state for the selected VF. Refer to section 8 for more
information.
     * 
     */
    uint8_t sss:1;
    /**
     * @brief Default = ‘0’. When the Controller Restore State (CRS) flag in
the USBCMD register is written with ‘1’ this bit shall be set to ‘1’ and remain 1 while the xHC
restores its internal state. When the Restore State operation is complete, this bit shall be
cleared to ‘0’. Refer to section 4.23.2 for more information.
When this register is exposed by a Virtual Function (VF), the VMM determines the state of this
bit as a function of the restoring the state for the selected VF. Refer to section 8 for more
information.
     * 
     */
    uint8_t rss:1;
    /**
     * @brief Default = ‘0’. If an error occurs during a Save or Restore
operation this bit shall be set to ‘1’. This bit shall be cleared to ‘0’ when a Save or Restore
operation is initiated or when written with ‘1’. Refer to section 4.23.2 for more information.
When this register is exposed by a Virtual Function (VF), the VMM determines the state of this
bit as a function of the Save/Restore completion status for the selected VF. Refer to section 8
for more information.
     * 
     */
    uint8_t sre:1;
    /**
     * @brief Default = ‘1’. ‘0’ = Ready and ‘1’ = Not Ready. Software shall
not write any Doorbell or Operational register of the xHC, other than the USBSTS register, until
CNR = ‘0’. This flag is set by the xHC after a Chip Hardware Reset and cleared when the xHC is
ready to begin accepting register writes. This flag shall remain cleared (‘0’) until the next Chip
Hardware Reset.
     * 
     */
    uint8_t cnr:1;
    /**
     * @brief Default = 0. 0’ = No internal xHC error conditions exist and ‘1’
= Internal xHC error condition. This flag shall be set to indicate that an internal error condition
has been detected which requires software to reset and reinitialize the xHC. Refer to section
4.24.1 for more information.
     * 
     */
    uint8_t hce:1;
    uint32_t rsvd3:19;
}__attribute__((packed)) XHCI_OPERATIONAL_REGISTER_USBSTS;

typedef struct{
     /**
      * @brief This bit identifies the value of the xHC Consumer Cycle State (CCS)
flag for the TRB referenced by the Command Ring Pointer. Refer to section 4.9.3 for more
information.
Writes to this flag are ignored if Command Ring Running (CRR) is ‘1’.
If the CRCR is written while the Command Ring is stopped (CRR = ‘0’), then the value of this flag
shall be used to fetch the first Command TRB the next time the Host Controller Doorbell register
is written with the DB Reason field set to Host Controller Command.
If the CRCR is not written while the Command Ring is stopped (CRR = ‘0’), then the Command
Ring shall begin fetching Command TRBs using the current value of the internal Command Ring
CCS flag.
Reading this flag always returns ‘0’.
      * 
      */
     uint8_t rcs:1;
     /**
      * @brief Default = ‘0’. Writing a ‘1’ to this bit shall stop the operation of the
Command Ring after the completion of the currently executing command, and generate a
Command Completion Event with the Completion Code set to Command Ring Stopped and the
Command TRB Pointer set to the current value of the Command Ring Dequeue Pointer. Refer to
section 4.6.1.1 for more information on stopping a command.
The next write to the Host Controller Doorbell with DB Reason field set to Host Controller
Command shall restart the Command Ring operation.
Writes to this flag are ignored by the xHC if Command Ring Running (CRR) = ‘0’.
Reading this bit shall always return ‘0’.
      * 
      */
     uint8_t cs:1;
     /**
      * @brief Default = ‘0’. Writing a ‘1’ to this bit shall immediately terminate
the currently executing command, stop the Command Ring, and generate a Command
Completion Event with the Completion Code set to Command Ring Stopped. Refer to section
4.6.1.2 for more information on aborting a command.
The next write to the Host Controller Doorbell with DB Reason field set to Host Controller
Command shall restart the Command Ring operation.
Writes to this flag are ignored by the xHC if Command Ring Running (CRR) = ‘0’.
Reading this bit always returns ‘0’.
      * 
      */
     uint8_t ca:1;
     /**
      * @brief Default = 0. This flag is set to ‘1’ if the Run/Stop (R/S) bit is
‘1’ and the Host Controller Doorbell register is written with the DB Reason field set to Host
Controller Command. It is cleared to ‘0’ when the Command Ring is “stopped” after writing a ‘1’
to the Command Stop (CS) or Command Abort (CA) flags, or if the R/S bit is cleared to ‘0’.
      * 
      */
     uint8_t crr:1;
     uint8_t rsvd1:2;
     /**
      * @brief Default = ‘0’. This field defines high order bits of the initial value
of the 64-bit Command Ring Dequeue Pointer.
Writes to this field are ignored when Command Ring Running (CRR) = ‘1’.
If the CRCR is written while the Command Ring is stopped (CRR = ‘0’), the value of this field shall
be used to fetch the first Command TRB the next time the Host Controller Doorbell register is
written with the DB Reason field set to Host Controller Command.
If the CRCR is not written while the Command Ring is stopped (CRR = ‘0’) then the Command
Ring shall begin fetching Command TRBs at the current value of the internal xHC Command
Ring Dequeue Pointer.
Reading this field always returns ‘0’.
      * 
      */
     uint32_t pointer:21;
}__attribute__((packed)) XHCI_OPERATIONAL_REGISTER_CRCR;

typedef struct{
     /**
      * @brief Default = ‘0’. This field specifies the maximum
number of enabled Device Slots. Valid values are in the range of 0 to MaxSlots. Enabled Devices
Slots are allocated contiguously. e.g. A value of 16 specifies that Device Slots 1 to 16 are active.
A value of ‘0’ disables all Device Slots. A disabled Device Slot shall not respond to Doorbell
Register references.
This field shall not be modified by software if the xHC is running (Run/Stop (R/S) = ‘1’).
      * 
      */
     uint8_t MaxSlotsEn:8;
     /**
      * @brief Default = '0'. When set to '1', the xHC shall assert the PLC flag ('1')
when a Root Hub port transitions to the U3 State. Refer to section 4.15.1 for more information.
      * 
      */
     uint8_t U3E:1;
     /**
      * @brief Default = '0'. When set to '1', the software shall
initialize the Configuration Value, Interface Number, and Alternate Setting fields in the Input
Control Context when it is associated with a Configure Endpoint Command. When this bit is '0',
the extended Input Control Context fields are not supported. Refer to section 6.2.5.1 for more
information.
      * 
      */
     uint8_t CIE:1;
     uint32_t rsvd:22;
}__attribute__((packed)) XHCI_OPERATIONAL_REGISTER_CONFIG;

typedef struct{
     uint8_t CCS:1;
     uint8_t PED:1;
     uint8_t rsvd1:1;
     uint8_t OCA:1;
     uint8_t PR:1;
     uint16_t PLS:4;
     uint8_t PP:1;
     uint16_t PortSpeed:4;
     uint8_t PIC:2;
     uint8_t LWS:1;
     uint8_t CSC:1;
     uint8_t PEC:1;
     uint8_t WRC:1;
     uint8_t OCC:1;
     uint8_t PRC:1;
     uint8_t PLC:1;
     uint8_t CEC:1;
     uint8_t CAS:1;
     uint8_t WCE:1;
     uint8_t WDE:1;
     uint8_t WOE:1;
     uint8_t rsvd2:2;
     uint8_t dr:1;
     uint8_t wpr:1;
}__attribute__((packed)) XHCI_OPERATIONAL_PORT_PORTSC;

typedef struct{
     uint8_t U1Timeout:8;
     uint8_t U2Timeout:8;
     uint8_t FLA:1;
     uint32_t reserved:15;
}__attribute__((packed)) XHCI_OPERATIONAL_PORT_PORTPMSC;

typedef struct{
     XHCI_OPERATIONAL_PORT_PORTSC PORTSC;
     XHCI_OPERATIONAL_PORT_PORTPMSC PORTPMSC;
     uint32_t portli;
     uint32_t porthlpmc;
}__attribute__((packed)) XHCI_OPERATIONAL_PORT_REGISTER;

typedef struct{
    XHCI_OPERATIONAL_REGISTER_USBCMD USBCMD;
    XHCI_OPERATIONAL_REGISTER_USBSTS USBSTS;
    uint32_t PAGESIZE;
    uint8_t RsvdZ1[0x8];
    uint32_t DNCTRL;
    uint32_t CRCR_low;
    uint32_t CRCR_high;
    uint8_t RsvdZ2[0x10];
    uint32_t DCBAAP_low;
    uint32_t DCBAAP_high;
    XHCI_OPERATIONAL_REGISTER_CONFIG CONFIG;
    uint8_t RsvdZ3[0x3C4];
    XHCI_OPERATIONAL_PORT_REGISTER ports[30];
}__attribute__((packed)) XHCI_OPERATIONAL_REGISTER;

typedef struct{
    uint8_t IP:1;
    uint8_t IE:1;
    uint32_t rsvd:29;
}__attribute__((packed)) XHCI_RUNTIME_REGISTER_IMAN;

typedef struct{
    uint16_t IMODI;
    uint16_t IMODC;
}__attribute__((packed)) XHCI_RUNTIME_REGISTER_IMOD;

typedef struct{
    XHCI_RUNTIME_REGISTER_IMAN iman;
    XHCI_RUNTIME_REGISTER_IMOD imod;
    uint32_t erstsz;
    uint32_t reserved;
    uint32_t erstba_lo;
    uint32_t erstba_hi;
    uint32_t erdp_lo;
    uint32_t erdp_hi;
}__attribute__((packed)) XHCIInterrupterRegisters;

typedef struct{
    uint32_t mfindex;
    uint8_t reserved[0x1C];
    XHCIInterrupterRegisters intregs[10];
}__attribute__((packed)) XHCI_RUNTIME_REGISTERS;

typedef struct{
    uint64_t address;
    uint32_t ringsegmentsize;
    uint32_t rsvd;
}__attribute__((packed)) XHCIEventRingSegmentTableEntry;


typedef struct{
    // TRB Pointer Hi and Lo. This field represents the 64-bit address of the TRB that generated this
    // event or 64 bits of Event Data if the ED flag is ‘1’.
    // If a TRB memory structure is referenced by this field (ED = ‘0’), then it shall be physical memory
    // pointer aligned on a 16-byte boundary, i.e. bits 0 through 3 of the address are ‘0’.
    uint64_t trb_pointer;
    // TRB Transfer Length. This field shall reflect the residual number of bytes not transferred.
    // For an OUT, this field shall indicate the value of the Length field of the Transfer TRB, minus the
    // data bytes that were successfully transmitted. A successful OUT transfer shall return a Length of
    // ‘0’.
    // For an IN, this field shall indicate the value of the TRB Transfer Length field of the Transfer TRB,
    // minus the data bytes that were successfully received. If the device terminates the receive
    // transfer with a Short Packet, then this field shall indicate the difference between the expected
    // transfer size (defined by the Transfer TRB) and the actual number of bytes received. If the
    // receive transfer completed with an error, then this field shall indicate the difference between
    // the expected transfer size and the number of bytes successfully received.
    // If the Event Data flag is ‘0’ the legal range of values is 0 to 10000h. If the Event Data flag is ‘1’ or
    // the Condition Code is Stopped - Short Packet, then this field shall be set to the value of the
    // Event Data Transfer Length Accumulator (EDTLA). Refer to section 4.11.5.2 for a description of
    // EDTLA.
    uint32_t transfer_length: 24;
    // Completion Code. This field encodes the completion status that can be identified by a TRB.
    // Refer to section 6.4.5 for an enumerated list of possible error conditions.
    uint32_t competion_code: 7;
    
    // Slot ID. The ID of the Device Slot that generated the event. This is value is used as an index in
    // the Device Context Base Address Array to select the Device Context of the source device.
    uint32_t slot_id: 7;
    uint32_t reserved_3: 3;
    // Endpoint ID. The ID of the Endpoint that generated the event. This value is used as an index in
    // the Device Context to select the Endpoint Context associated with this event.
    uint32_t endpoint_id: 5;
    // TRB Type. This field identifies the type of the TRB. Refer to Table 6-91 for the definition of the
    // Transfer Event TRB type ID.
    uint32_t trb_type: 6;
    uint32_t reserved_2: 7;
    // Event Data (ED). When set to ‘1’, the event was generated by an Event Data TRB and the
    // Parameter Component (TRB Pointer field) contains a 64-bit value provided by the Event Data
    // TRB. If cleared to ‘0’, the Parameter Component (TRB Pointer field) contains a pointer to the TRB
    // that generated this event. Refer to section 4.11.5.2 for more information.
    uint32_t event_data: 1;
    // RsvdZ.
    uint32_t reserved_1: 1;
    // Cycle bit (C). This bit is used to mark the Dequeue Pointer of an Event Ring.
    uint32_t cycle_bit: 1;
}__attribute__((packed))TransferEventTRB;

typedef struct{
    uint32_t reserved[3];
    uint8_t cycle_bit:1;
    uint16_t reserved1:9;
    uint16_t trb_type:6;
    uint16_t slot_type:5;
    uint16_t reserved2:10;
}__attribute__((packed))EnableSlotTRB;

typedef struct{
    uint32_t arg1;
    uint32_t arg2;
    uint32_t arg3;
    uint32_t arg4;
}__attribute__((packed)) TRB;

typedef struct{
    // Command TRB Pointer Hi and Lo. This field represents the high order bits of the 64-bit address
    // of the Command TRB that generated this event. Note that this field is not valid for some
    // Completion Code values. Refer to Table 6-90 for specific cases.
    // The memory structure referenced by this physical memory pointer shall be aligned on a 16-byte
    // address boundary.
    uint64_t CommandTRBPointer;
    // Command Completion Parameter. This field may optionally be set by a command. Refer to
    // section 4.6.6.1 for specific usage. If a command does not utilize this field it shall be treated as
    // RsvdZ.
    uint32_t CommandCompletionParameter: 24;
    // Completion Code. This field encodes the completion status of the command that generated the
    // event. Refer to the respective command definition for a list of the possible Completion Codes
    // associated with the command. Refer to section 6.4.5 for an enumerated list of possible error
    // conditions.
    uint32_t CompletionCode:8;
    // This bit is used to mark the Dequeue Pointer of an Event Ring.
    uint8_t Cyclebit:1;
    uint16_t reserved1:9;
    // TRB Type. This field identifies the type of the TRB. Refer to Table 6-91 for the definition of the
    // Command Completion Event TRB type ID.
    uint16_t TRBType:6;
    // VF ID. The ID of the Virtual Function that generated the event. Note that this field is valid only if
    // Virtual Functions are enabled. If they are not enabled this field shall be cleared to ‘0’.
    uint16_t VFID:8;
    // Slot ID. The Slot ID field shall be updated by the xHC to reflect the slot associated with the
    // command that generated the event, with the following exceptions:
    // - The Slot ID shall be cleared to ‘0’ for No Op, Set Latency Tolerance Value, Get Port Bandwidth,
    // and Force Event Commands.
    // - The Slot ID shall be set to the ID of the newly allocated Device Slot for the Enable Slot
    // Command.
    // - The value of Slot ID shall be vendor defined when generated by a vendor defined command.
    // This value is used as an index in the Device Context Base Address Array to select the Device
    // Context of the source device. If this Event is due to a Host Controller Command, then this field
    // shall be cleared to ‘0’.
    uint16_t SlotID:8;
}__attribute__((packed)) CommandCompletionEventTRB;

typedef struct{
    uint32_t usbcmd;
    uint32_t usbsts;
    uint32_t pagesize;
    uint8_t rsvdA[0x08];
    uint32_t dnctrl;
    uint64_t crcr;
    uint8_t rsvdB[0x10];
    uint64_t dcbaap;
}__attribute__((packed)) XHCIOperationalRegisters;

void initialise_xhci_driver(unsigned long bar,unsigned long usbint);