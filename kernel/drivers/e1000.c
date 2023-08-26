#include "../include/e1000.h"
#include "../include/ethernet.h"
#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/pci.h"
#include "../include/idt.h"
#include "../include/memory.h"

unsigned long base_addr;
unsigned char is_eeprom;
unsigned char mac_address[8];
unsigned volatile long e1000_package_recieved_ack = 0;
int rx_cur;
int tx_cur;
volatile struct e1000_rx_desc *rx_descs[E1000_NUM_RX_DESC];
volatile struct e1000_tx_desc *tx_descs[E1000_NUM_TX_DESC];

extern void e1000irq();

void e1000_write_in_space(unsigned short addr,unsigned long val){
    *( (volatile unsigned int *)(base_addr + addr)) = val;
}

unsigned long e1000_read_in_space(unsigned short addr){
    return *( (volatile unsigned int *)(base_addr + addr));
}

unsigned int e1000_read_command(unsigned short addr){
    unsigned long adt = base_addr;
    unsigned int *t = (unsigned int*) adt;
    return t[addr];
}

unsigned char e1000_is_eeprom(){
    for(int i = 0 ; i < 1000 ; ++i){
        unsigned long to = e1000_read_command(0x14);
        if((to&0x10)==0x10){
            return 1;
        }
    }
    return 0;
}

__attribute__((interrupt)) void irq_e1000(interrupt_frame *frame){
    e1000_write_in_space(0xD0,1);
    unsigned long to = e1000_read_in_space(0xC0);
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
    if(to&0x01){
        // k_printf("[E1000] Transmit completed!\n");
    }else if(to&0x02){
        k_printf("[E1000] Transmit queue empty!\n");
    }else if(to&0x04){
        k_printf("[E1000] Link change!\n");
    }else if(to&0x80){
        // k_printf("[E1000] Package recieved!\n");
        PackageRecievedDescriptor prd;
		for(int i = 0 ; i < E1000_NUM_RX_DESC ; i++){
            if((rx_descs[i]->status & 0x1))
            {
                unsigned char *buf = (unsigned char *)rx_descs[i]->addr_1;
                unsigned short len = rx_descs[i]->length;
                prd.buffersize = len;
                prd.high_buf = 0;
                prd.low_buf = (unsigned long)buf;
                if(ethernet_handle_package(prd)){
                    rx_descs[i]->status &= ~1;
                    e1000_write_in_space(REG_RXDESCTAIL, i );
                }
            }
        }
    }else if(to&0x10){
        k_printf("[E1000] THG!\n");
    }else{
        k_printf("[E1000] Unknown interrupt: %x !\n",to);
    }
}

int e1000_send_package(PackageRecievedDescriptor desc){
    int old = tx_cur;
    tx_descs[tx_cur]->addr_1 = (unsigned long)desc.low_buf;
    tx_descs[tx_cur]->addr_2 = (unsigned long)desc.high_buf;
    tx_descs[tx_cur]->length = desc.buffersize;
    tx_descs[tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
    tx_descs[tx_cur]->status = 0;
    tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
    e1000_write_in_space(REG_TXDESCTAIL, tx_cur);
    while(!tx_descs[old]->status);
    return 1;
}

void e1000_enable_int(){
    e1000_write_in_space(0xD0,0x1F6DC);
    e1000_write_in_space(0xD0,0xff & ~4);
    e1000_read_in_space(0xC0);
    k_printf("[E1000] Interrupts enabled!\n");
}

void e1000_link_up(){
    unsigned long ty = e1000_read_in_space(0);
    e1000_write_in_space(0, ty | 0x40);
    k_printf("[E1000] Link is up!\n");
}

PackageRecievedDescriptor e1000_recieve_package(){
    PackageRecievedDescriptor prd;
    while(1){
        for(int i = 0 ; i < E1000_NUM_RX_DESC ; i++){
            if((rx_descs[i]->status & 0x1))
            {
                unsigned char *buf = (unsigned char *)rx_descs[i]->addr_1;
                unsigned short len = rx_descs[i]->length;
                prd.buffersize = len;
                prd.high_buf = 0;
                prd.low_buf = (unsigned long)buf;

                rx_descs[i]->status &= ~1;
                e1000_write_in_space(REG_RXDESCTAIL, i );
                return prd;
            }
        }
    }
}

void e1000_driver_start(int bus,int slot,int function){
    k_printf("[E1000] E1000 initialised bus=%x slot=%x function=%x \n",bus,slot,function);
    base_addr = getBARaddress(bus,slot,function,0x10) & 0xFFFFFFFE;
    k_printf("[E1000] Base address: %x \n",base_addr);
	unsigned long usbint = getBARaddress(bus,slot,function,0x3C) & 0x000000FF;
    k_printf("[E1000] USBINT %x \n",usbint);
    setInterrupt(usbint,irq_e1000);

    //
    // mac ophalen
    is_eeprom = e1000_is_eeprom();
    if(is_eeprom){
        k_printf("[E1000] Device has EEPROM\n");
        k_printf("[E1000] EEPROM is not supported, yet\n");
        return;
    }else{
        k_printf("[E1000] Device has NO EEPROM\n");
        unsigned long *tg = (unsigned long*) (base_addr + 0x5400);
        mac_address[0] = ((tg[0] & 0x000000FF)>>0) & 0xFF;
        mac_address[1] = ((tg[0] & 0x0000FF00)>>8) & 0xFF;
        mac_address[2] = ((tg[0] & 0x00FF0000)>>16) & 0xFF;
        mac_address[3] = ((tg[0] & 0xFF000000)>>24) & 0xFF;
        mac_address[4] = ((tg[1] & 0x000000FF)>>0) & 0xFF;
        mac_address[5] = ((tg[1] & 0x0000FF00)>>8) & 0xFF;
    }
    k_printf("[E1000] MAC: %x:%x:%x:%x:%x:%x \n",mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]);

    if(!(getBARaddress(bus,slot,function,0x04)&0x04)){
        unsigned long to = pciConfigReadWord(bus,slot,function,0x04) | 0x04;
        pciConfigWriteWord(bus,slot,function,0x04,to);
        k_printf("[E1000] Busmastering was not enabled, but now it is!\n");
    }


    // no multicast
    for(int i = 0; i < 0x80; i++){
        e1000_write_in_space(0x5200 + (i * 4), 0);
    }
    e1000_write_in_space(0xD0,0x1F6DC);
    e1000_read_in_space(0xC0);

    //
    // set reciever
    unsigned char * ptr;
    struct e1000_rx_desc *descs;
 
    ptr = (unsigned char *)requestPage();
 
    descs = (struct e1000_rx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        rx_descs[i] = (struct e1000_rx_desc *)((unsigned char *)descs + i*16);
        rx_descs[i]->addr_1 = (unsigned long)(unsigned char *)requestPage();
        rx_descs[i]->status = 0;
        rx_descs[i]->length = 0x3000;
    }
 
    e1000_write_in_space(REG_TXDESCLO, (unsigned long) ptr);
    e1000_write_in_space(REG_TXDESCHI, 0);
 
    e1000_write_in_space(REG_RXDESCLO, (unsigned long)ptr);
    e1000_write_in_space(REG_RXDESCHI, 0);
 
    e1000_write_in_space(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);
 
    e1000_write_in_space(REG_RXDESCHEAD, 0);
    e1000_write_in_space(REG_RXDESCTAIL, 31);
    rx_cur = 0;
    e1000_write_in_space(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);
    
    //
    // sending
    unsigned char *  ptr2;
    struct e1000_tx_desc *descs2;
    ptr2 = (unsigned char *)requestPage();
 
    descs2 = (struct e1000_tx_desc *)ptr2;
    for(int i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        tx_descs[i] = (struct e1000_tx_desc *)((unsigned char*)descs2 + i*16);
        tx_descs[i]->addr_1 = (unsigned long)requestPage();
        tx_descs[i]->cmd = 0;
        tx_descs[i]->length = 0x3000;
        tx_descs[i]->status = TSTA_DD;
    }
 
    e1000_write_in_space(REG_TXDESCLO, (unsigned long)ptr2 );
    e1000_write_in_space(REG_TXDESCHI, 0);
 
    e1000_write_in_space(REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);
 
    e1000_write_in_space( REG_TXDESCHEAD, 0);
    e1000_write_in_space( REG_TXDESCTAIL, 7);
    tx_cur = 0;
    e1000_write_in_space(0x3828,  (0x01000000 | 0x003F0000));
    e1000_write_in_space(REG_TCTRL,  ( 0x00000ff0 | 0x003ff000 | 0x00000008 | 0x00000002));
    e1000_write_in_space(REG_TIPG,  0x0060200A);

    //
    // set interrupts
    e1000_enable_int();
    e1000_link_up();

    //
    // register driver
    register_ethernet_device((unsigned long)&e1000_send_package,(unsigned long)&e1000_recieve_package,mac_address);
}