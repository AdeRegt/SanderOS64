#include "driver.h"

#define MAC_SIZE 6

__attribute__((interrupt)) void irq_rtl(interrupt_frame* frame){

	k_printf("rtl: interrupt recieved!\n");

	outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

 struct Descriptor
 {
     uint32_t command;  /* command/status uint32_t */
     uint32_t vlan;     /* currently unused */
     uint32_t low_buf;  /* low 32-bits of physical buffer address */
     uint32_t high_buf; /* high 32-bits of physical buffer address */
 };
 
 /** 
  * Note that this assumes 16*1024=16KB (4 pages) of physical memory at 1MB and 2MB is identity mapped to 
  * the same linear address range
  */
 struct Descriptor *Rx_Descriptors; /* 1MB Base Address of Rx Descriptors */
 struct Descriptor *Tx_Descriptors; /* 2MB Base Address of Tx Descriptors */
 
 unsigned long num_of_rx_descriptors = 1024, num_of_tx_descriptors = 1024;
 unsigned long rx_buffer_len = 100;
 
 void setup_rx_descriptors()
 {
    /* rx_buffer_len is the size (in bytes) that is reserved for incoming packets */
    unsigned int OWN = 0x80000000, EOR = 0x40000000; /* bit offsets */
    int i;
    for(i = 0; i < num_of_rx_descriptors; i++) /* num_of_rx_descriptors can be up to 1024 */
    {
        if(i == (num_of_rx_descriptors - 1)) /* Last descriptor? if so, set the EOR bit */
          Rx_Descriptors[i].command = (OWN | EOR | (rx_buffer_len & 0x3FFF));
        else
          Rx_Descriptors[i].command = (OWN | (rx_buffer_len & 0x3FFF));

        /** packet_buffer_address is the *physical* address for the buffer */
        Rx_Descriptors[i].low_buf = (unsigned int)0x300000;
        Rx_Descriptors[i].high_buf = 0;
        /* If you are programming for a 64-bit OS, put the high memory location in the 'high_buf' descriptor area */
    }
}

void driver_start(PCIInfo *pci){
	k_printf("rtl: Initialising RTL Driver at BAR %x and int at %d \n",pci->bar1,pci->inter);

	// read the MAC address
	k_printf("rtl: Our MAC is "); 
	uint8_t mac_address[MAC_SIZE];
	for(uint8_t i = 0 ; i < MAC_SIZE ; i++){
		mac_address[i] = inportb(pci->bar1 + i) & 0xFF;
		k_printf("%x ",mac_address[i] & 0xFF);
	}

	k_printf("\nrtl: Starting reset...");
	outportb(pci->bar1 + 0x37 , 0x10);
    sleep(3);
	while(inportb(pci->bar1 + 0x37) & 0x10);
	k_printf("OK\n");

    memset((void*)&Tx_Descriptors[0],0,sizeof(struct Descriptor)*num_of_rx_descriptors);
    memset((void*)&Rx_Descriptors[0],0,sizeof(struct Descriptor)*num_of_rx_descriptors);

    setup_rx_descriptors();
    outportb(pci->bar1 + 0x50, 0xC0); /* Unlock config registers */
    outportl(pci->bar1 + 0x44, 0x0000E70F); /* RxConfig = RXFTH: unlimited, MXDMA: unlimited, AAP: set (promisc. mode set) */
    outportb(pci->bar1 + 0x37, 0x04); /* Enable Tx in the Command register, required before setting TxConfig */
    outportl(pci->bar1 + 0x40, 0x03000700); /* TxConfig = IFG: normal, MXDMA: unlimited */
    outportw(pci->bar1 + 0xDA, 0x1FFF); /* Max rx packet size */
    outportb(pci->bar1 + 0xEC, 0x3B); /* max tx packet size */
 
    /* offset 0x20 == Transmit Descriptor Start Address Register 
       offset 0xE4 == Receive Descriptor Start Address Register 
 
       Again, these are *physical* addresses. This code assumes physical==linear, this is
       typically not the case in real world kernels
    */
    outportl(pci->bar1 + 0x20, (unsigned long)&Tx_Descriptors[0]); /* Tell the NIC where the first Tx descriptor is. NOTE: If writing a 64-bit address, split it into two 32-bit writes.*/
    outportl(pci->bar1 + 0xE4, (unsigned long)&Rx_Descriptors[0]); /* Tell the NIC where the first Rx descriptor is. NOTE: If writing a 64-bit address, split it into two 32-bit writes.*/
 
    outportb(pci->bar1 + 0x37, 0x0C); /* Enable Rx/Tx in the Command register */
    outportb(pci->bar1 + 0x50, 0x00); /* Lock config registers */
    
	k_printf("OK?");
    setInterrupt(pci->inter,irq_rtl);
    for(;;);
}