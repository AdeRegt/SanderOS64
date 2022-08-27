#pragma once 
#include <stdint.h>

typedef struct{
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
}interrupt_frame;

typedef struct{
    int bus;
    int slot;
    int function;
    uint32_t bar1;
    int inter;
}__attribute__((packed)) PCIInfo;

typedef struct{
	uint32_t buffersize;
	void* buffer;
}PackageRecievedDescriptor;

#define SIZE_OF_MAC 6
#define SIZE_OF_IP 4

void outportb(uint16_t port, uint8_t value);                            // !
uint8_t inportb(uint16_t port);                                         // !

void outportw(uint16_t port, uint16_t value);                           // !
uint16_t inportw(uint16_t port);                                        // !

void outportl(uint16_t port, uint32_t value);                           // !
uint32_t inportl(uint16_t port);                                        // !

void k_printf(char* format,...);                                        // !

void setInterrupt(int offset,void *fun);                                // !
void *requestPage();
void memset(void *start, unsigned char value, unsigned long long num);  // !

unsigned long getBARaddress(int bus,int slot,int function,int barNO);   // !
void sleep(uint64_t time);                                              // !
void registerHIDDevice(void *getcpointer);                              // !
void *requestPage();

void ethernet_set_link_status(unsigned long a);
int ethernet_handle_package(PackageRecievedDescriptor desc);
void register_ethernet_device(void *sendPackage,void *recievePackage,unsigned char mac[SIZE_OF_MAC]);