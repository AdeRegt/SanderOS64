#include "../include/xhci.h"
#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/timer.h"
#include "../include/idt.h"
#include "../include/memory.h"

XHCI_CAPABILITY_REGISTER *capabilities;
XHCI_OPERATIONAL_REGISTER *operations;
XHCI_RUNTIME_REGISTERS *runtime;
uint32_t *doorbells;
uint32_t *dcbaap;

void *eventring;
void *commandring;

uint8_t xhci_get_trb_result_of(TRB* erc){
    return (erc->arg3>>24)&0xFF;
}

uint8_t xhci_get_trb_type_of(TRB* erc){
    return (erc->arg4>>10)&0x3F;
}

uint32_t xhci_get_memory_dword(void* addr){
    uint32_t *tv = (uint32_t*) addr;
    return tv[0];
}

void xhci_set_memory_dword(void* addr,uint32_t value){
    uint32_t *tv = (uint32_t*) addr;
    tv[0] = value;
}

__attribute__((interrupt)) void irq_xhci(interrupt_frame* frame){

    asm volatile("cli");

    k_printf("xhci: interrupt detected!\n");

    if(operations->USBSTS.eint){
        k_printf("-> this is a event interrupt\n");
        operations->USBSTS.eint = 1;
    }else if(operations->USBSTS.pcd){
        k_printf("-> this is a port interrupt\n");
        operations->USBSTS.pcd = 1;
    }else{
        k_printf("-> this is a unknown interrupt: %x \n",operations->USBSTS);
    }

    XHCIInterrupterRegisters *r1 = (XHCIInterrupterRegisters*) (&runtime->intregs[0]);
    r1->iman.IP = 1;
    xhci_set_memory_dword((void*)&r1->erdp_lo,xhci_get_memory_dword((void*)&r1->erdp_lo)|8);

	outportb(0xA0,0x20);
	outportb(0x20,0x20);

    asm volatile("sti");
}

uint32_t command_ring_pointer = 0;

void xhci_dump_event_ring(){
    for(int i = 0 ; i < 5 ; i++){
        TRB* erc = (TRB*) (eventring + (sizeof(TRB)*i));
        uint8_t check = erc->arg4&1;
        // if(check){
            k_printf("%d: 1:%x 2:%x 3:%x 4:%x z:%d\n",i,erc->arg1,erc->arg2,erc->arg3,erc->arg4,check);
        // } 
    }
}

TRB* xhci_wait_for_event(TRB* origin){
    while(1){
        for(int i = 0 ; i < 16 ; i++){
            TRB* erc = (TRB*) (eventring + (sizeof(TRB)*i));
            if( ((uint32_t)(((uint64_t)origin)&0xFFFFFFFF))==erc->arg1 && erc->arg4&1 ){
                return erc;
            } 
        }
    }
    return 0;
}

uint16_t xhci_get_get_port_number(){
    EnableSlotTRB* est = (EnableSlotTRB*)(commandring + (sizeof(TRB) * command_ring_pointer++));
    est->cycle_bit = 1;
    est->trb_type = 9;
    doorbells[0] = 0;
    CommandCompletionEventTRB* completion_event = (CommandCompletionEventTRB*) xhci_wait_for_event((TRB*)est);
    if(!completion_event){
        xhci_dump_event_ring();
        k_printf("xhci: no event found yet (timeout)!\n");
        return 0;
    }
    if(completion_event->CompletionCode!=1){
        return 0;
    }
    return completion_event->SlotID;
}

uint16_t xhci_port_get_address(int pid,uint8_t speed,uint16_t devicespeed,uint64_t addr,uint8_t bsr,uint8_t slotid){
    XHCI_INPUT_CONTEXT* ipc = (XHCI_INPUT_CONTEXT*) requestPage();
    memset(ipc,0,sizeof(XHCI_INPUT_CONTEXT));
    ipc->icc.A_row = 3;

    ipc->slot.RootHubPortNumber = pid + 1;
    ipc->slot.ContextEntries = 1;
    ipc->slot.Speed = speed;

    ipc->ec0.EPType = 4;
    ipc->ec0.CErr = 3;
    ipc->ec0.MaxPackageSize = devicespeed;
    ipc->ec0.DequeuePointer = ((uint64_t)addr) | 1;

    k_printf("ga: basis:%x icc:%x slot:%x ec0:%x \n",ipc,&ipc->icc,&ipc->slot,&ipc->ec0);

    AddressDeviceCommandTRB *est = (AddressDeviceCommandTRB*)(commandring + (sizeof(TRB) * command_ring_pointer++));
    est->c = 1;
    est->BSR = bsr;
    est->TRBType = 11;
    est->SlotID = slotid;
    est->address_low = (uint32_t) ((uint64_t)ipc);

    doorbells[(slotid*2)+0] = (uint32_t) ((uint64_t)ipc+64);
    doorbells[(slotid*2)+1] = 0;

    doorbells[0] = 0;
    CommandCompletionEventTRB* completion_event = (CommandCompletionEventTRB*) xhci_wait_for_event((TRB*)est);
    k_printf("result: %d \n",completion_event->CompletionCode);
    return 0;
}

void xhci_install_port(XHCI_OPERATIONAL_PORT_REGISTER *port,int pid){
    
    k_printf("xhci_%d: Something is on this port cas:%d ccs:%d cec:%d csc:%d dr:%d lws:%d oca:%d occ:%d ped:%d pic:%d plc:%d pls:%d ps:%d pp:%d pr:%d prc:%d wce:%d wde:%d woe:%d wpr:%d wrc:%d ...\n",
        pid,
        port->PORTSC.CAS,
        port->PORTSC.CCS,
        port->PORTSC.CEC,
        port->PORTSC.CSC,
        port->PORTSC.dr,
        port->PORTSC.LWS,
        port->PORTSC.OCA,
        port->PORTSC.OCC,
        port->PORTSC.PED,
        port->PORTSC.PIC,
        port->PORTSC.PLC,
        port->PORTSC.PLS,
        port->PORTSC.PortSpeed,
        port->PORTSC.PP,
        port->PORTSC.PR,
        port->PORTSC.PRC,
        port->PORTSC.WCE,
        port->PORTSC.WDE,
        port->PORTSC.WOE,
        port->PORTSC.wpr,
        port->PORTSC.WRC
    );

    while(1){
        uint32_t pi = xhci_get_memory_dword((void*)&port->PORTSC);
        if(((pi>>5)&0b1111)!=7){
            break;
        }
    }

    void *localring = requestPage();
    memset(localring,0,80 * sizeof(TRB) );

    uint16_t portnumber = xhci_get_get_port_number();
    k_printf("xhci_%d: Portnumber: %d Portspeed: %d \n",pid,portnumber,port->PORTSC.PortSpeed);
    xhci_port_get_address(pid,port->PORTSC.PortSpeed,8,(uint64_t)localring,1,portnumber);
}

uint8_t xhci_test_command_ring(){
    // no-op command
    TRB* est = (TRB*)(commandring + (sizeof(TRB) * command_ring_pointer++));
    est->arg4 = (23<<10) | 1;
    ((volatile uint32_t*)&doorbells[0])[0] = 0;
    TRB* res = xhci_wait_for_event(est);
    if(!res){
        k_printf("Command ring test failed! \n");
        return 0;
    }
    if( xhci_get_trb_type_of(res)==XHCI_TRB_TYPE_EVENT_COMMAND_COMPLETION_EVENT &&xhci_get_trb_result_of(res)==1 ){
        k_printf("xhci: command ring test succeed!\n");
        return 1;
    }
    k_printf("Command ring test failed with trb type %d and resultcode %d \n",xhci_get_trb_type_of(res),xhci_get_trb_result_of(res));
    return 0;
}

void initialise_xhci_driver(unsigned long bar,unsigned long usbint){
    k_printf("xhci: arived at XHCI driver, BAR=%x , INT=%d \n",bar,usbint);
    setInterrupt(usbint,irq_xhci);
    capabilities = (XHCI_CAPABILITY_REGISTER*) bar;
    operations = (XHCI_OPERATIONAL_REGISTER*) ( bar + capabilities->CAPLENGTH );
    runtime = (XHCI_RUNTIME_REGISTERS*) (bar + capabilities->RTSOFF);
    doorbells = (uint32_t*)(bar + capabilities->DBOFF);
    
    k_printf("xhci: max_int: %d max_ports: %d max_slots: %d \n",capabilities->HCSPARAMS1.MaxIntrs,capabilities->HCSPARAMS1.MaxPorts,capabilities->HCSPARAMS1.MaxSlots);
    k_printf("xhci: ERSTMax: %d IST:%d MaxScratchpadBufs: %d %d SPR: %d \n",capabilities->HCSPARAMS2.ERSTMax,capabilities->HCSPARAMS2.IST,capabilities->HCSPARAMS2.MaxScratchpadBufsHi,capabilities->HCSPARAMS2.MaxScratchpadBufsLo,capabilities->HCSPARAMS2.SPR);
    k_printf("xhci: ac64:%d bnc:%d cfc:%d csz:%d lhrc:%d ltc:%d maxpsasize:%d nss:%d pae:%d pind:%d ppc:%d sec:%d spc:%d xecp:%d \n",capabilities->HCCPARAMS1.ac64,capabilities->HCCPARAMS1.bnc,capabilities->HCCPARAMS1.cfc,capabilities->HCCPARAMS1.csz,capabilities->HCCPARAMS1.lhrc,capabilities->HCCPARAMS1.ltc,capabilities->HCCPARAMS1.maxpsasize,capabilities->HCCPARAMS1.nss,capabilities->HCCPARAMS1.pae,capabilities->HCCPARAMS1.pind,capabilities->HCCPARAMS1.ppc,capabilities->HCCPARAMS1.sec,capabilities->HCCPARAMS1.spc,capabilities->HCCPARAMS1.xecp);
    k_printf("xhci: cic:%d cmc:%d ctc:%d etc:%d etc_tsc:%d fsc:%d gsc:%d lec:%d res:%d u3c:%d vtc:%d \n",capabilities->HCCPARAMS2.CIC,capabilities->HCCPARAMS2.CMC,capabilities->HCCPARAMS2.CTC,capabilities->HCCPARAMS2.ETC,capabilities->HCCPARAMS2.ETC_TSC,capabilities->HCCPARAMS2.FSC,capabilities->HCCPARAMS2.GSC,capabilities->HCCPARAMS2.LEC,capabilities->HCCPARAMS2.res,capabilities->HCCPARAMS2.U3C,capabilities->HCCPARAMS2.VTC);
    k_printf("xhci: rs:%d inte:%d hsee:%d ewe:%d eu3s:%d cme:%d ete:%d tse:%d \n",operations->USBCMD.run_stop,operations->USBCMD.inte,operations->USBCMD.hsee,operations->USBCMD.ewe,operations->USBCMD.eu3s,operations->USBCMD.cme,operations->USBCMD.ete,operations->USBCMD.tsc_en);

    // 
    // are we running ?
    if(operations->USBCMD.run_stop){
        k_printf("xhci: We are still running! stop it!\n");
        operations->USBCMD.run_stop = 0;
        sleep(10);
        while(1){
            if(operations->USBSTS.hch==1){
                break;
            }
        }
    }

    //
    // lets trigger a reset and see if we survive...
    operations->USBCMD.hcrst = 1;
    while(1){
        sleep(10);
        if(operations->USBCMD.hcrst==0){
            break;
        }
    }
    while(operations->USBSTS.cnr==1);
    k_printf("xhci: finished the reset!\n");

    // set max used ports
    operations->CONFIG.MaxSlotsEn = capabilities->HCSPARAMS1.MaxSlots;

    // create eventring
    eventring = requestPage();
    memset(eventring,0,80*sizeof(TRB));

    // create event ring handler
    XHCIEventRingSegmentTableEntry *erste = (XHCIEventRingSegmentTableEntry*) requestPage();
    memset(erste,0,sizeof(XHCIEventRingSegmentTableEntry));
    erste->address = (uint32_t) ((uint64_t) eventring);
    erste->ringsegmentsize = 16;

    // setup event ring registers
    XHCIInterrupterRegisters *r1 = (XHCIInterrupterRegisters*) (&runtime->intregs[0]);
    r1->erstsz = 1;
    r1->erdp_lo = (uint32_t) ((uint64_t) eventring);
    r1->erdp_hi = 0;
    r1->erstba_lo = (uint32_t) ((uint64_t) erste);
    r1->erstba_hi = 0;
    // r1->imod.IMODC = 0;
    // r1->imod.IMODI = 4000;

    // enable interrupts
    // operations->USBCMD.inte = 1;
    // while(operations->USBSTS.cnr==1);
    
    // // enable wrap events
    // operations->USBCMD.ewe = 1;

    // enable interrupter 1
    r1->iman.IE = 1;
    while(operations->USBSTS.cnr==1);

    // create commandring
    commandring = requestPage();
    memset(commandring,0,80 * sizeof(TRB) );

    // set commandring
    operations->CRCR_low = ((uint32_t) ((uint64_t) commandring)) | 1;
    operations->CRCR_high = 0;

    // create dcbaap ring
    dcbaap = (uint32_t*)requestPage();
    memset(dcbaap,0,sizeof(uint32_t)*1000);

    operations->DCBAAP_low = (uint32_t)((uint64_t)dcbaap);
    operations->DCBAAP_high = 0;
    // operations->DCBAAP_low = dcbaap_old_addrB;
    // operations->DCBAAP_high = dcbaap_old_addrA;

    k_printf("xhci: Start the system!\n");
    while(operations->USBSTS.cnr==1);

    operations->USBCMD.run_stop = 1;

    // test everything
    if(!xhci_test_command_ring()){
        return;
    }

    // waiting untill we have a new port
    while(1){
        if(operations->USBSTS.pcd){
            break;
        }
    }

    for(uint8_t i = 0 ; i < operations->CONFIG.MaxSlotsEn ; i++){
        XHCI_OPERATIONAL_PORT_REGISTER *port = (XHCI_OPERATIONAL_PORT_REGISTER*) (&operations->ports[i]);
        if(port->PORTSC.CSC){
            xhci_install_port(port,i);;
        }
    }

    k_printf("All finished...\n");
    for(;;);
}