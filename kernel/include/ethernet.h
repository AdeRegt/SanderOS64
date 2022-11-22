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
    uint8_t to[SIZE_OF_MAC];
    uint8_t from[SIZE_OF_MAC];
    uint16_t type;
} __attribute__ ((packed));

struct ARPHeader{
    struct EthernetHeader ethernetheader;
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_address_length;
    uint8_t protocol_address_length;
    uint16_t operation;

    uint8_t source_mac[SIZE_OF_MAC];
    uint8_t source_ip[SIZE_OF_IP];

    uint8_t dest_mac[SIZE_OF_MAC];
    uint8_t dest_ip[SIZE_OF_IP];
} __attribute__ ((packed));

struct IPv4Header{
    struct EthernetHeader ethernetheader;
    uint8_t internet_header_length:4;
    uint8_t version:4;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags:3;
    uint16_t fragment_offset:13;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t source_addr;
    uint32_t dest_addr;
} __attribute__ ((packed));

struct ICMPHeader{
    struct IPv4Header ipv4header;
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t ident_BE;
    uint16_t ident_LE;
    uint16_t seqe_BE;
    uint16_t seqe_LE;
    uint8_t timestamp[8];
} __attribute__ ((packed));

struct UDPHeader{
    struct IPv4Header ipv4header;
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__ ((packed));

struct TCPHeader{
    struct IPv4Header header;
    uint16_t source_port;
    uint16_t destination_port;
    uint32_t sequence_number;
    uint32_t acknowledge_number;
    uint16_t flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_pointer;
} __attribute__ ((packed));

struct TFTPAcknowledgeHeader{
    struct UDPHeader header;
    uint16_t type;
    uint16_t index;
} __attribute__ ((packed));

struct DHCPDISCOVERHeader{
    struct UDPHeader udpheader;
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t timing;
    uint16_t flags;
    uint32_t address_of_machine;
    uint32_t dhcp_offered_machine;
    uint32_t ip_addr_of_dhcp_server;
    uint32_t ip_addr_of_relay;
    uint8_t client_mac_addr [16];
    uint8_t sname [64];
    uint8_t file [128];
    uint32_t magic_cookie;
    uint8_t options[76];
} __attribute__ ((packed));

struct DHCPREQUESTHeader{
    struct UDPHeader udpheader;
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t timing;
    uint16_t flags;
    uint32_t address_of_machine;
    uint32_t dhcp_offered_machine;
    uint32_t ip_addr_of_dhcp_server;
    uint32_t ip_addr_of_relay;
    uint8_t client_mac_addr [16];
    uint8_t sname [64];
    uint8_t file [128];
    uint32_t magic_cookie;
    uint8_t options[25];
} __attribute__ ((packed));

struct DNSREQUESTHeader{
    struct UDPHeader udpheader;
    uint16_t transaction_id;
    uint16_t flags;
    uint16_t question_count;
    uint16_t answer_rr;
    uint16_t authority_rr;
    uint16_t aditional_rr;
} __attribute__ ((packed));

void ethernet_detect(int bus,int slot,int function,int device,int vendor);
void ethernet_set_link_status(uint32_t a);
void register_ethernet_device(void *sendPackage,void *recievePackage,uint8_t mac[SIZE_OF_MAC]);
EthernetDevice getDefaultEthernetDevice();
PackageRecievedDescriptor getEthernetPackage();
int sendEthernetPackage(PackageRecievedDescriptor desc);
void initialise_ethernet();
int ethernet_handle_package(PackageRecievedDescriptor desc);
uint8_t* getMACFromIp(uint8_t* ip);
uint8_t* getOurRouterIp();
void create_tcp_session(uint32_t from, uint32_t to, uint16_t from_port, uint16_t to_port, upointer_t func);
uint8_t* getIPFromName(char* name);
void fillUdpHeader(struct UDPHeader *udpheader, uint8_t *destmac, uint16_t size,uint32_t from, uint32_t to,uint16_t source_port, uint16_t destination_port);
void fillTcpHeader(struct TCPHeader *tcpheader,uint8_t *destmac,uint16_t size,uint32_t from,uint32_t to,uint16_t from_port,uint16_t to_port,uint32_t sequence_number,uint32_t acknowledge_number,uint8_t header_length,uint16_t flags,uint16_t window);
uint32_t getOurIpAsLong();
PackageRecievedDescriptor getEthernetPackage();
uint16_t switch_endian16(uint16_t nb);
uint32_t switch_endian32(uint32_t num);