#include "driver.h"

#define SIZE_OF_MAC 6
#define SIZE_OF_IP 4
//
// FROM: https://wiki.osdev.org/RTL8169
// Technical specification: http://realtek.info/pdf/rtl8139d.pdf
// This is a entry in the queue of the recieve and transmit descriptor queue
#define OWN 0x80000000
#define EOR 0x40000000
struct Descriptor{
	uint32_t command;  /* command/status uint32_t */
	uint32_t vlan;     /* currently unused */
	void *buffer;
};

void init_rtl(int bus,int slot,int function);

//
// How to run networking on qemu:
// See main-tread: https://forum.osdev.org/viewtopic.php?f=1&t=30546
// Requires: sudo apt-get install bridge-utils
// sudo brctl addbr br0
// sudo ../../qemu/x86_64-softmmu/qemu-system-x86_64 -netdev tap,id=thor_net0 -device rtl8139,netdev=thor_net0,id=thor_nic0 -kernel ../kernel.bin
//

// The recieve and transmit queue
struct Descriptor *Rx_Descriptors; /* 1MB Base Address of Rx Descriptors */
struct Descriptor *Tx_Descriptors; /* 2MB Base Address of Tx Descriptors */

// For loops
uint32_t num_of_rx_descriptors = 1024;
uint32_t num_of_tx_descriptors = 1024;
uint64_t bar1 = 0;
uint32_t rx_pointer = 0;
uint32_t tx_pointer = 0;
uint32_t package_recieved_ack = 0;
uint32_t package_send_ack = 0;


__attribute__((interrupt)) void irq_rtl8169(interrupt_frame* frame){
	k_printf("[RTL81] Interrupt detected\n");
	unsigned short status = inportw(bar1 + 0x3E);
	if(status&0x20){
		k_printf("[RTL81] Link change detected!\n");
		ethernet_set_link_status(1);
		status |= 0x20;
	}
	if(status&0x01){
		k_printf("[RTL81] Package recieved!\n");
		PackageRecievedDescriptor prd;
		for(int z = 0 ; z < 100 ; z++){
			if(!(Rx_Descriptors[z].command & OWN)){
				prd.buffersize = Rx_Descriptors[z].command & 0x3FFF;
				prd.buffer = Rx_Descriptors[z].buffer;
				if(ethernet_handle_package(prd)){
					Rx_Descriptors[z].command |= OWN;
				}
			}
		}
		status |= 0x01;
	}
	if(status&0x04){
		k_printf("[RTL81] Package send!\n");
		((unsigned volatile long*)((unsigned volatile long)&package_send_ack))[0] = 1;
		status |= 0x04;
	}
	outportw(bar1 + 0x3E,status);
	
	status = inportw(bar1 + 0x3E);
	if(status!=0x00){
		k_printf("[RTL81] Unresolved interrupt: %x \n",status);
	}
	
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

void rtl_sendPackage(PackageRecievedDescriptor desc){
	uint32_t ms1 = 0x80000000 | 0x40000000 | 0x40000 | 0x20000000 | 0x10000000 | (desc.buffersize & 0x3FFF); // 0x80000000 | ownbit=yes | firstsegment | lastsegment | length
	uint32_t ms2 = 0 ;
	void *ms3 = desc.buffer;
	
	volatile struct Descriptor *desz;
	oa:
	desz = ((volatile struct Descriptor*)(Tx_Descriptors+(sizeof(struct Descriptor)*tx_pointer)));
	if(desz->command!=0x80000064){ // a check if we somehow lost the count
		k_printf("[RTL81] Unexpected default value: %x \n",desz->command);
	}
	desz->buffer = ms3;
	desz->vlan = ms2;
	desz->command = ms1;
	
	((unsigned volatile long*)((unsigned volatile long)&package_send_ack))[0] = 0;
	outportb(bar1 + 0x38, 0x40); // ring the doorbell
	
	while(1){ // wait for int or end of polling
		unsigned volatile long x = ((unsigned volatile long*)((unsigned volatile long)&package_send_ack))[0];
		if(x==1){
			break;
		}
		unsigned volatile char poller = inportb(bar1 + 0x38);
		if((poller&0x40)==0){
			break;
		}
	}
	((unsigned volatile long*)((unsigned volatile long)&package_send_ack))[0] = 0;
}

PackageRecievedDescriptor rtl_recievePackage(){
	PackageRecievedDescriptor prd;
	uint64_t time = 0;
	while(time<0x500000){
		int i = -1;
		for(int z = 0 ; z < 100 ; z++){
			if(!(Rx_Descriptors[z].command & OWN)){
				Rx_Descriptors[z].command |= OWN;
				prd.buffersize = Rx_Descriptors[z].command & 0x3FFF;
				prd.buffer = Rx_Descriptors[z].buffer;
				i = z;
				break;
			}
		}
		if(i!=-1){
			break;
		}
		time++;
	}
	return prd;
}

void rtl_test(){
	PackageRecievedDescriptor res = rtl_recievePackage();
	k_printf("[RTL81] Testpackage recieved. Length=%x \n",res.buffersize);
	
	PackageRecievedDescriptor resx;
	resx.buffersize = res.buffersize;
	resx.buffer = res.buffer;
	rtl_sendPackage(resx);
	k_printf("[RTL81] Package send\n");
}

int driver_start(PCIInfo *pi){
    k_printf("[RTL81] Driver loaded\n");
	bar1 = getBARaddress(pi->bus,pi->slot,pi->function,0x10) & 0xFFFFFFFE;
	k_printf("[RTL81] BAR=%x \n",bar1);
	
	k_printf("[RTL81] Set interrupter\n");
	unsigned long usbint = getBARaddress(pi->bus,pi->slot,pi->function,0x3C) & 0x000000FF;
	setInterrupt(usbint,irq_rtl8169);

	// enable device
	// outportb( bar1 + 0x52, 0x0);
	
	//
	// trigger reset
	k_printf("[RTL81] Resetting driver \n");
	outportb(bar1 + 0x37, 0x10);
	sleep(5);
	while(inportb(bar1 + 0x37) & 0x10){}
	k_printf("[RTL81] Driver reset succesfully \n");
	
	//
	// get mac address
	uint8_t macaddress[SIZE_OF_MAC];
	for(int i = 0 ; i < SIZE_OF_MAC ; i++){
		macaddress[i] = inportb(bar1+i);
	}
	k_printf("[RTL81] MAC-address %x %x %x %x %x %x \n",macaddress[0],macaddress[1],macaddress[2],macaddress[3],macaddress[4],macaddress[5]);

	requestPage();
	Rx_Descriptors = (struct Descriptor*)requestPage();
	Tx_Descriptors = (struct Descriptor*)requestPage();
	memset(Rx_Descriptors,0,num_of_rx_descriptors*sizeof(struct Descriptor));
	memset(Tx_Descriptors,0,num_of_rx_descriptors*sizeof(struct Descriptor));
	
	//
	// setup rx descriptor
	k_printf("[RTL81] Setup RX descriptor\n");
	for(unsigned long i = 0; i < num_of_rx_descriptors; i++){
		unsigned long rx_buffer_len = 100;
		void *packet_buffer_address = requestPage();
		if(i == (num_of_rx_descriptors - 1)){
			Rx_Descriptors[i].command = (OWN | EOR | (rx_buffer_len & 0x3FFF));
		}else{
			Rx_Descriptors[i].command = (OWN | (rx_buffer_len & 0x3FFF));
		}
		Rx_Descriptors[i].buffer = packet_buffer_address;
	}
	
	outportb(bar1 + 0x50, 0xC0); /* Unlock config registers */
	outportl(bar1 + 0x44, 0x0000E70F); /* RxConfig = RXFTH: unlimited, MXDMA: unlimited, AAP: set (promisc. mode set) */
	outportb(bar1 + 0x37, 0x04); /* Enable Tx in the Command register, required before setting TxConfig */
	outportl(bar1 + 0x40, 0x03000700); /* TxConfig = IFG: normal, MXDMA: unlimited */
	outportw(bar1 + 0xDA, 0x1FFF); /* Max rx packet size */
	outportb(bar1 + 0xEC, 0x3B); /* max tx packet size */ // 0x3B
	outportl(bar1 + 0x20, (uint32_t)((uint64_t)Tx_Descriptors)); /* Tell the NIC where the first Tx descriptor is */
	outportl(bar1 + 0x24, (uint32_t)((uint64_t)(Tx_Descriptors) >> 32)); 
	outportl(bar1 + 0xE4, (uint32_t)((uint64_t)Rx_Descriptors)); /* Tell the NIC where the first Rx descriptor is */
	outportl(bar1 + 0xE8, (uint32_t)((uint64_t)(Rx_Descriptors) >> 32));
	outportw(bar1 + 0x3C, 0xC1FF); /* Set all masks open so we get much ints */
	outportb(bar1 + 0x37, 0x0C); /* Enable Rx/Tx in the Command register */
	outportb(bar1 + 0x50, 0x00); /* Lock config registers */
	
	register_ethernet_device(rtl_sendPackage,rtl_recievePackage,macaddress);
	k_printf("[RTL81] Setup finished\n");
}