#pragma once 
#include <stdint.h>
#include "outint.h"

#define SIZE_OF_MAC 6
#define SIZE_OF_IP 4
#define ETHERNET_TYPE_ARP 0x0608
#define ETHERNET_TYPE_IP4 0x0008
#define IPV4_TYPE_UDP 0x11
#define IPV4_TYPE_TCP 0x06
#define IPV4_TYPE_ICMP 0x01
#define TCP_FIN 0b000000000001
#define TCP_SYN 0b000000000010
#define TCP_RES 0b000000000100
#define TCP_PUS 0b000000001000
#define TCP_ACK 0b000000010000
#define TCP_URG 0b000000100000
#define TCP_ECN 0b000001000000
#define TCP_CWR 0b000010000000
#define TCP_NON 0b000100000000

typedef struct{
	uint32_t buffersize;
	void* buffer;
}PackageRecievedDescriptor;

typedef struct{
	void* sendPackage;
	void* recievePackage;
	uint8_t is_enabled;
	uint8_t mac[SIZE_OF_MAC];
	volatile uint32_t is_online;
}EthernetDevice;

struct EthernetHeader{
    unsigned char to[SIZE_OF_MAC];
    unsigned char from[SIZE_OF_MAC];
    unsigned short type;
} __attribute__ ((packed));

struct ARPHeader{
    struct EthernetHeader ethernetheader;
    unsigned short hardware_type;
    unsigned short protocol_type;
    unsigned char hardware_address_length;
    unsigned char protocol_address_length;
    unsigned short operation;

    unsigned char source_mac[SIZE_OF_MAC];
    unsigned char source_ip[SIZE_OF_IP];

    unsigned char dest_mac[SIZE_OF_MAC];
    unsigned char dest_ip[SIZE_OF_IP];
} __attribute__ ((packed));

struct IPv4Header{
    struct EthernetHeader ethernetheader;
    unsigned char internet_header_length:4;
    unsigned char version:4;
    unsigned char type_of_service;
    unsigned short total_length;
    unsigned short id;
    unsigned short flags:3;
    unsigned short fragment_offset:13;
    unsigned char time_to_live;
    unsigned char protocol;
    unsigned short checksum;
    unsigned long source_addr;
    unsigned long dest_addr;
} __attribute__ ((packed));

struct ICMPHeader{
    struct IPv4Header ipv4header;
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned short ident_BE;
    unsigned short ident_LE;
    unsigned short seqe_BE;
    unsigned short seqe_LE;
    unsigned char timestamp[8];
} __attribute__ ((packed));

struct UDPHeader{
    struct IPv4Header ipv4header;
    unsigned short source_port;
    unsigned short destination_port;
    unsigned short length;
    unsigned short checksum;
} __attribute__ ((packed));

struct TCPHeader{
    struct IPv4Header header;
    unsigned short source_port;
    unsigned short destination_port;
    unsigned long sequence_number;
    unsigned long acknowledge_number;
    unsigned short flags;
    unsigned short window_size;
    unsigned short checksum;
    unsigned short urgent_pointer;
} __attribute__ ((packed));

struct TFTPAcknowledgeHeader{
    struct UDPHeader header;
    unsigned short type;
    unsigned short index;
} __attribute__ ((packed));

struct DHCPDISCOVERHeader{
    struct UDPHeader udpheader;
    unsigned char op;
    unsigned char htype;
    unsigned char hlen;
    unsigned char hops;
    unsigned long xid;
    unsigned short timing;
    unsigned short flags;
    unsigned long address_of_machine;
    unsigned long dhcp_offered_machine;
    unsigned long ip_addr_of_dhcp_server;
    unsigned long ip_addr_of_relay;
    unsigned char client_mac_addr [16];
    unsigned char sname [64];
    unsigned char file [128];
    unsigned long magic_cookie;
    unsigned char options[76];
} __attribute__ ((packed));

struct DHCPREQUESTHeader{
    struct UDPHeader udpheader;
    unsigned char op;
    unsigned char htype;
    unsigned char hlen;
    unsigned char hops;
    unsigned long xid;
    unsigned short timing;
    unsigned short flags;
    unsigned long address_of_machine;
    unsigned long dhcp_offered_machine;
    unsigned long ip_addr_of_dhcp_server;
    unsigned long ip_addr_of_relay;
    unsigned char client_mac_addr [16];
    unsigned char sname [64];
    unsigned char file [128];
    unsigned long magic_cookie;
    unsigned char options[25];
} __attribute__ ((packed));

struct DNSREQUESTHeader{
    struct UDPHeader udpheader;
    unsigned short transaction_id;
    unsigned short flags;
    unsigned short question_count;
    unsigned short answer_rr;
    unsigned short authority_rr;
    unsigned short aditional_rr;
} __attribute__ ((packed));

void ethernet_detect(int bus,int slot,int function,int device,int vendor);
void ethernet_set_link_status(unsigned long a);
void register_ethernet_device(void *sendPackage,void *recievePackage,unsigned char mac[SIZE_OF_MAC]);
EthernetDevice getDefaultEthernetDevice();
PackageRecievedDescriptor getEthernetPackage();
int sendEthernetPackage(PackageRecievedDescriptor desc);
void initialise_ethernet();
int ethernet_handle_package(PackageRecievedDescriptor desc);
unsigned char* getMACFromIp(unsigned char* ip);
unsigned char* getOurRouterIp();
void create_tcp_session(unsigned long from, unsigned long to, unsigned short from_port, unsigned short to_port, unsigned long func);
unsigned char* getIPFromName(char* name);
void fillUdpHeader(struct UDPHeader *udpheader, unsigned char *destmac, unsigned short size,unsigned long from, unsigned long to,unsigned short source_port, unsigned short destination_port);
void fillTcpHeader(struct TCPHeader *tcpheader,unsigned char *destmac,unsigned short size,unsigned long from,unsigned long to,unsigned short from_port,unsigned short to_port,unsigned long sequence_number,unsigned long acknowledge_number,unsigned char header_length,unsigned short flags,unsigned short window);
unsigned long getOurIpAsLong();
PackageRecievedDescriptor getEthernetPackage();
unsigned short switch_endian16(unsigned short nb);
unsigned long switch_endian32(unsigned long num);