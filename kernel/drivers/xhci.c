#include "../include/xhci.h"
#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/timer.h"
#include "../include/idt.h"

XHCI_CAPABILITY_REGISTER *capabilities;
XHCI_OPERATIONAL_REGISTER *operations;

__attribute__((interrupt)) void irq_xhci(interrupt_frame* frame){
    k_printf("xhci: interrupt detected!\n");
	
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

void initialise_xhci_driver(unsigned long bar,unsigned long usbint){
    k_printf("xhci: arived at XHCI driver, BAR=%x , INT=%d \n",bar,usbint);
    setInterrupt(usbint,irq_xhci);
    capabilities = (XHCI_CAPABILITY_REGISTER*) bar;
    operations = (XHCI_OPERATIONAL_REGISTER*) ( bar + capabilities->CAPLENGTH );
    k_printf("xhci: max_int: %d max_ports: %d max_slots: %d \n",capabilities->HCSPARAMS1.MaxIntrs,capabilities->HCSPARAMS1.MaxPorts,capabilities->HCSPARAMS1.MaxSlots);
    k_printf("xhci: ERSTMax: %d IST:%d MaxScratchpadBufs: %d %d SPR: %d \n",capabilities->HCSPARAMS2.ERSTMax,capabilities->HCSPARAMS2.IST,capabilities->HCSPARAMS2.MaxScratchpadBufsHi,capabilities->HCSPARAMS2.MaxScratchpadBufsLo,capabilities->HCSPARAMS2.SPR);
    k_printf("xhci: ac64:%d bnc:%d cfc:%d csz:%d lhrc:%d ltc:%d maxpsasize:%d nss:%d pae:%d pind:%d ppc:%d sec:%d spc:%d xecp:%d \n",capabilities->HCCPARAMS1.ac64,capabilities->HCCPARAMS1.bnc,capabilities->HCCPARAMS1.cfc,capabilities->HCCPARAMS1.csz,capabilities->HCCPARAMS1.lhrc,capabilities->HCCPARAMS1.ltc,capabilities->HCCPARAMS1.maxpsasize,capabilities->HCCPARAMS1.nss,capabilities->HCCPARAMS1.pae,capabilities->HCCPARAMS1.pind,capabilities->HCCPARAMS1.ppc,capabilities->HCCPARAMS1.sec,capabilities->HCCPARAMS1.spc,capabilities->HCCPARAMS1.xecp);
    k_printf("xhci: cic:%d cmc:%d ctc:%d etc:%d etc_tsc:%d fsc:%d gsc:%d lec:%d res:%d u3c:%d vtc:%d \n",capabilities->HCCPARAMS2.CIC,capabilities->HCCPARAMS2.CMC,capabilities->HCCPARAMS2.CTC,capabilities->HCCPARAMS2.ETC,capabilities->HCCPARAMS2.ETC_TSC,capabilities->HCCPARAMS2.FSC,capabilities->HCCPARAMS2.GSC,capabilities->HCCPARAMS2.LEC,capabilities->HCCPARAMS2.res,capabilities->HCCPARAMS2.U3C,capabilities->HCCPARAMS2.VTC);
    
    //
    // lets trigger a reset and see if we survive...
    operations->USBCMD.hcrst = 1;
    while(1){
        sleep(10);
        if(operations->USBCMD.hcrst==0){
            break;
        }
    }
    k_printf("xhci: finished the reset!\n");

    // enable interrupts
    operations->USBCMD.inte = 1;
    
    // enable wrap events
    operations->USBCMD.ewe = 1;

    // set max used ports
    operations->CONFIG.MaxSlotsEn = capabilities->HCSPARAMS1.MaxSlots;

    k_printf("All finished...\n");
    for(;;);
}