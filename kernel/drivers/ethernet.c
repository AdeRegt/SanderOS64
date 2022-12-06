#include "../include/ethernet.h"
#include "../include/graphics.h"
#include "../include/memory.h"


uint16_t switch_endian16(uint16_t nb) {
    return (nb>>8) | (nb<<8);
}

uint32_t switch_endian32(uint32_t num) {
    return ((num>>24)&0xff) | // move byte 3 to byte 0
    ((num<<8)&0xff0000) | // move byte 1 to byte 2
    ((num>>8)&0xff00) | // move byte 2 to byte 1
    ((num<<24)&0xff000000); // byte 0 to byte 3
}

// void ethernet_detect(int bus,int slot,int function,int device,int vendor){
//     if((device==0x8168||device==0x8139)&&vendor==0x10ec){ 
//         // Sander his RTL8169 driver comes here
//         init_rtl(bus,slot,function);
//     }else if(device==0x100e||device==0x153A||device==0x10EA||vendor==0x8086){
//         // Johan his E1000 driver comes here
//         init_e1000(bus,slot,function);
//     }else{
//         printf("[ETH] Unknown ethernet device: device: %x vendor: %x \n",device,vendor);
//     }
// }

EthernetDevice defaultEthernetDevice;
uint8_t our_ip[SIZE_OF_IP];
uint8_t router_ip[SIZE_OF_IP];
uint8_t dns_ip[SIZE_OF_IP];
uint8_t dhcp_ip[SIZE_OF_IP];

void ethernet_set_link_status(uint32_t a){
    defaultEthernetDevice.is_online = a;
}

void register_ethernet_device(void *sendPackage,void *recievePackage,uint8_t mac[SIZE_OF_MAC]){
    defaultEthernetDevice.recievePackage = recievePackage;
    defaultEthernetDevice.sendPackage = sendPackage;
    defaultEthernetDevice.is_enabled = 1;
    for(int i = 0 ; i < SIZE_OF_MAC ; i++){
        defaultEthernetDevice.mac[i] = mac[i];
    }
}

EthernetDevice getDefaultEthernetDevice(){
    return defaultEthernetDevice;
}

PackageRecievedDescriptor getEthernetPackage(){
    PackageRecievedDescriptor (*getPackage)() = (void*)defaultEthernetDevice.recievePackage;
    return getPackage();
}

int sendEthernetPackage(PackageRecievedDescriptor desc){
    int (*sendPackage)(PackageRecievedDescriptor desc) = (void*)defaultEthernetDevice.sendPackage;
    return sendPackage(desc);
}

void fillMac(uint8_t* to,uint8_t* from){
    for(int i = 0 ; i < SIZE_OF_MAC ; i++){
        to[i] = from[i];
    }
}

void fillIP(uint8_t* to,uint8_t* from){
    for(int i = 0 ; i < SIZE_OF_IP ; i++){
        to[i] = from[i];
    }
}

void fillEthernetHeader(struct EthernetHeader* eh, uint8_t* destip,uint16_t type){
    fillMac((uint8_t*)&eh->to,destip);
    fillMac((uint8_t*)&eh->from,(uint8_t*)&defaultEthernetDevice.mac);
    eh->type = type;
}

uint8_t* getMACFromIp(uint8_t* ip){
    struct ARPHeader* arpie = (struct ARPHeader*)malloc(sizeof(struct ARPHeader));
    uint8_t everyone[SIZE_OF_MAC] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    uint8_t empty[SIZE_OF_MAC] = {0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t prefip[SIZE_OF_IP] = {0,0,0,0};
    memcpy(prefip,our_ip,4);
    fillEthernetHeader((struct EthernetHeader*) &arpie->ethernetheader,everyone,ETHERNET_TYPE_ARP);
    arpie->hardware_type = 0x0100;
    arpie->protocol_type = 0x0008;
    arpie->hardware_address_length = SIZE_OF_MAC;
    arpie->protocol_address_length = SIZE_OF_IP;
    arpie->operation = 0x0100;

    fillMac((uint8_t*)&arpie->source_mac,(uint8_t*)&defaultEthernetDevice.mac);
    fillIP((uint8_t*)&arpie->source_ip,(uint8_t*)&prefip);

    fillMac((uint8_t*)&arpie->dest_mac,(uint8_t*)&empty);
    fillIP((uint8_t*)&arpie->dest_ip,ip);
    
    PackageRecievedDescriptor sec;
    sec.buffersize = sizeof(struct ARPHeader);
    sec.buffer = arpie;

    sendEthernetPackage(sec);
    struct ARPHeader* ah;
    while(1){
        PackageRecievedDescriptor prd = getEthernetPackage();
        struct EthernetHeader *eh = (struct EthernetHeader*) prd.buffer;
        if(eh->type==ETHERNET_TYPE_ARP){
            ah = (struct ARPHeader*) prd.buffer;
            if(ah->operation==0x0200){
                break;
            }
        }
    }
    k_printf("[ETH] %d.%d.%d.%d is at %x:%x:%x:%x:%x:%x \n",ip[0],ip[1],ip[2],ip[3],ah->source_mac[0],ah->source_mac[1],ah->source_mac[2],ah->source_mac[3],ah->source_mac[4],ah->source_mac[5]);
    return ah->source_mac;
}

uint8_t* getOurRouterIp(){
    return (uint8_t*) &router_ip;
}

uint16_t ipv4count = 1;
void fillIpv4Header(struct IPv4Header *ipv4header, uint8_t* destmac, uint16_t length,uint8_t protocol,uint32_t from, uint32_t to){
    fillEthernetHeader((struct EthernetHeader*)&ipv4header->ethernetheader,destmac,ETHERNET_TYPE_IP4);
    ipv4header->version = 4;
    ipv4header->internet_header_length = 5;
    ipv4header->type_of_service = 0;
    ipv4header->total_length = switch_endian16( length );
    ipv4header->id = switch_endian16(ipv4count);
    ipv4header->flags = 0;
    ipv4header->fragment_offset= 0b01000;
    ipv4header->time_to_live = 64;
    ipv4header->protocol = protocol;
    ipv4header->checksum = 0;
    ipv4header->source_addr = from;
    ipv4header->dest_addr = to;

    uint32_t checksum = 0;
    checksum += 0x4500;
    checksum += length;
    checksum += ipv4count++;
    checksum += 0x4000;
    checksum += 0x4000 + protocol;
    checksum += switch_endian16((from >> 16) & 0xFFFF);
    checksum += switch_endian16(from & 0xFFFF); 
    checksum += switch_endian16((to >> 16) & 0xFFFF);
    checksum += switch_endian16(to & 0xFFFF);
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);
    ipv4header->checksum = switch_endian16((uint16_t) (~checksum));
}

void fillUdpHeader(struct UDPHeader *udpheader, uint8_t *destmac, uint16_t size,uint32_t from, uint32_t to,uint16_t source_port, uint16_t destination_port){
    fillIpv4Header((struct IPv4Header*)&udpheader->ipv4header,destmac,size,IPV4_TYPE_UDP,from,to);

    udpheader->length = switch_endian16(size - (sizeof(struct IPv4Header)-sizeof(struct EthernetHeader)));
    udpheader->destination_port = switch_endian16(destination_port);
    udpheader->source_port = switch_endian16(source_port);

    udpheader->checksum = 0;
}

// checksum for tcp by https://github.com/nelsoncole/sirius-x86-64/blob/main/kernel/driver/net/checksum.c 
struct tcp_checksum_header{
    unsigned int src;
    unsigned int dst;
    uint8_t rsved;
    uint8_t protocol;
    uint16_t len;
    uint16_t source_port;
    uint16_t destination_port;
    uint32_t sequence_number;
    uint32_t acknowledge_number;
    uint16_t flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_pointer;
}__attribute__((packed));

uint16_t net_checksum(const uint8_t *start, const uint8_t *end)
{

    unsigned int checksum = 0;
    unsigned int len = end - start;
    uint16_t *p = (uint16_t *)start;

    // acc
    while (len > 1) {
        checksum += *p++;
        len -= 2;
    }

    if (len != 0) {
        checksum += *(uint8_t *)p;
    }


    checksum = (checksum & 0xffff) + (checksum >> 16);
    checksum += (checksum >> 16);

    uint16_t final = ~checksum;

    return switch_endian16(final);
}

uint32_t taaaX = 0;
uint32_t taaaY = 0;
void fillTcpHeader(struct TCPHeader *tcpheader,uint8_t *destmac,uint16_t size,uint32_t from,uint32_t to,uint16_t from_port,uint16_t to_port,uint32_t sequence_number,uint32_t acknowledge_number,uint8_t header_length,uint16_t flags,uint16_t window){
    fillIpv4Header((struct IPv4Header*)&tcpheader->header,destmac,size,IPV4_TYPE_TCP,from,to);
    if(sequence_number==0x1010&&acknowledge_number==0x1010){
        sequence_number = taaaX;
        acknowledge_number = taaaY;
        taaaX += (size + sizeof(struct EthernetHeader)) - sizeof(struct TCPHeader);
    }else if(flags&TCP_ACK){
        taaaX = sequence_number;
        taaaY = acknowledge_number;
    }
    tcpheader->source_port          = switch_endian16(from_port);
    tcpheader->destination_port     = switch_endian16(to_port);
    tcpheader->sequence_number      = switch_endian32(sequence_number);
    tcpheader->acknowledge_number   = switch_endian32(acknowledge_number);
    tcpheader->flags                = switch_endian16(flags) + (header_length<<4);
    tcpheader->window_size          = switch_endian16(window);
    tcpheader->checksum             = 0;
    tcpheader->urgent_pointer       = 0;

    int payload = ( size - (sizeof(struct TCPHeader) - sizeof(struct EthernetHeader)) );
    uint8_t *start = (uint8_t*)malloc(sizeof(struct tcp_checksum_header) + payload);
    uint8_t *end = start;
    end += (sizeof(struct tcp_checksum_header));

    struct tcp_checksum_header* trx = (struct tcp_checksum_header*)start;
    trx->dst = (to);
    trx->src = (from);
    trx->len = switch_endian16(20 + payload);
    trx->protocol = IPV4_TYPE_TCP;
    trx->source_port          = switch_endian16(from_port);
    trx->destination_port     = switch_endian16(to_port);
    trx->sequence_number      = switch_endian32(sequence_number);
    trx->acknowledge_number   = switch_endian32(acknowledge_number);
    trx->flags                = switch_endian16(flags) + (header_length<<4);
    trx->window_size          = switch_endian16(window);
    trx->checksum             = 0;
    trx->urgent_pointer       = 0;

    tcpheader->checksum = switch_endian16(net_checksum(start, end));
}

void fillDhcpDiscoverHeader(struct DHCPDISCOVERHeader *dhcpheader){
    uint8_t destmac[SIZE_OF_MAC] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    uint16_t size = sizeof(struct DHCPDISCOVERHeader) - sizeof(struct EthernetHeader);
    fillUdpHeader((struct UDPHeader*)&dhcpheader->udpheader,(uint8_t*)&destmac,size,0,0xFFFFFFFF,68,67);
}

void fillDhcpRequestHeader(struct DHCPREQUESTHeader *dhcpheader){
    uint8_t destmac[SIZE_OF_MAC] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    uint16_t size = sizeof(struct DHCPREQUESTHeader) - sizeof(struct EthernetHeader);
    fillUdpHeader((struct UDPHeader*)&dhcpheader->udpheader,(uint8_t*)&destmac,size,0,0xFFFFFFFF,68,67);
}

uint8_t* getIpAddressFromDHCPServer(){
    struct DHCPDISCOVERHeader *dhcpheader = (struct DHCPDISCOVERHeader *)malloc(sizeof(struct DHCPDISCOVERHeader));
    dhcpheader->op = 1;
    dhcpheader->htype = 1;
    dhcpheader->hlen = 6;
    dhcpheader->hops = 0;
    dhcpheader->xid = 0x26F30339;
    dhcpheader->timing = 0;
    dhcpheader->flags = switch_endian16(0x8000);

    fillMac((uint8_t*)&dhcpheader->client_mac_addr,(uint8_t*)&defaultEthernetDevice.mac);
    dhcpheader->magic_cookie = 0x63538263;
    // DHCP Message Type
    dhcpheader->options[0] = 0x35;
    dhcpheader->options[1] = 0x01;
    dhcpheader->options[2] = 0x01;
    // parameter request list
    dhcpheader->options[3] = 0x37;
    dhcpheader->options[4] = 0x40;
    dhcpheader->options[5] = 0xfc;
    for(uint8_t i = 1 ; i < 0x43 ; i++){
        dhcpheader->options[5+i] = i;
    }
    // dhcpheader->options[68] = 0x00;
    // ip address lease time
    dhcpheader->options[69] = 0x33;
    dhcpheader->options[70] = 0x04;
    dhcpheader->options[71] = 0x00;
    dhcpheader->options[72] = 0x00;
    dhcpheader->options[73] = 0x00;
    dhcpheader->options[74] = 0x01;
    // end
    dhcpheader->options[75] = 0xFF;
    
    fillDhcpDiscoverHeader(dhcpheader);
    PackageRecievedDescriptor sec;
    sec.buffersize = sizeof(struct DHCPDISCOVERHeader);
    sec.buffer = dhcpheader;
    int res_fs = sendEthernetPackage(sec); // send package
    if(res_fs==0){
        return 0;
    }
    PackageRecievedDescriptor prd;
    while(1){
        prd = getEthernetPackage(); 
        if(prd.buffer==0){
            return 0;
        }
        struct EthernetHeader *eh = (struct EthernetHeader*) prd.buffer;
        if(eh->type==ETHERNET_TYPE_IP4){
            struct IPv4Header *ip = (struct IPv4Header*) eh;
            if(ip->protocol==IPV4_TYPE_UDP){
                struct DHCPDISCOVERHeader *hd5 = ( struct DHCPDISCOVERHeader*) eh;
                if(switch_endian16(hd5->udpheader.destination_port)==68){
                    break;
                }
            }
        }
    }
    k_printf("[ETH] Got offer\n");
    struct DHCPDISCOVERHeader *hd = ( struct DHCPDISCOVERHeader*) prd.buffer;
    uint8_t* offeredip = (uint8_t*) &hd->dhcp_offered_machine;
    k_printf("[ETH] We are talking to: %s \n",hd->sname);
    int a = 0;
    while(1){
        uint8_t t = hd->options[a++];
        uint8_t z = hd->options[a++];
        if(t==0xFF||t==0x00){
            break;
        }else if( t==0x03){ // router
            uint8_t* re = (uint8_t*)&hd->options[a];
            fillIP((uint8_t*)&router_ip,re);
        }else if (t==0x06 ){ // dns
            uint8_t* re = (uint8_t*)&hd->options[a];
            fillIP((uint8_t*)&dns_ip,re);
        }else if (t==54 ){ // dhcp
            uint8_t* re = (uint8_t*)&hd->options[a];
            fillIP((uint8_t*)&dhcp_ip,re);
        }
        a += z;
    }

    free(dhcpheader);

    struct DHCPREQUESTHeader *dhcp2header = (struct DHCPREQUESTHeader *)malloc(sizeof(struct DHCPREQUESTHeader));
    dhcp2header->op = 1;
    dhcp2header->htype = 1;
    dhcp2header->hlen = 6;
    dhcp2header->hops = 0;
    dhcp2header->xid = 0x2CF30339;
    dhcp2header->timing = 0;
    dhcp2header->flags = switch_endian16(0x8000);

    fillMac((uint8_t*)&dhcp2header->client_mac_addr,(uint8_t*)&defaultEthernetDevice.mac);
    dhcp2header->magic_cookie = 0x63538263;

    // DHCP Message Type
    dhcp2header->options[0] = 0x35;
    dhcp2header->options[1] = 0x01;
    dhcp2header->options[2] = 0x03;
    // Client identifier
    dhcp2header->options[3] = 0x3d;
    dhcp2header->options[4] = 0x07;
    dhcp2header->options[5] = 0x01;
    fillMac((uint8_t*)(&dhcp2header->options)+6,(uint8_t*)&defaultEthernetDevice.mac);
    // Requested IP addr
    dhcp2header->options[12] = 0x32;
    dhcp2header->options[13] = 0x04;
    fillMac((uint8_t*)(&dhcp2header->options)+14,offeredip);
    // DHCP Server identifier
    dhcp2header->options[18] = 0x36;
    dhcp2header->options[19] = 0x04;
    fillMac((uint8_t*)(&dhcp2header->options)+20,(uint8_t*)&hd->ip_addr_of_dhcp_server);
    dhcp2header->options[24] = 0xFF;

    fillDhcpRequestHeader(dhcp2header);

    PackageRecievedDescriptor s3c;
    s3c.buffersize = sizeof(struct DHCPREQUESTHeader);
    s3c.buffer = dhcp2header;
    sendEthernetPackage(s3c); // send package
    PackageRecievedDescriptor p3d;
    while(1){
        p3d = getEthernetPackage(); 
        if(p3d.buffer==0){
            return 0;
        }
        struct EthernetHeader *eh = (struct EthernetHeader*) p3d.buffer;
        if(eh->type==ETHERNET_TYPE_IP4){
            struct IPv4Header *ip = (struct IPv4Header*) eh;
            if(ip->protocol==IPV4_TYPE_UDP){
                struct DHCPDISCOVERHeader *hd5 = ( struct DHCPDISCOVERHeader*) eh;
                if(switch_endian16(hd5->udpheader.destination_port)==68&&hd5->op==2&&hd5->xid==dhcp2header->xid){
                    break;
                }
            }
        }
    }
    k_printf("[ETH] Got Approval\n");

    return offeredip;
}

volatile uint16_t dnstid = 0xe0e0;
uint8_t* getIPFromName(char* name){
    k_printf("[ETH] Looking for IP of %s \n",name);
    int str = strlen(name);
    int ourheadersize = sizeof(struct DNSREQUESTHeader)+str+2+4;
    struct DNSREQUESTHeader *dnsreqheader = (struct DNSREQUESTHeader*) malloc(ourheadersize);
    uint8_t *destmac = getMACFromIp((uint8_t*)&dns_ip);
    uint16_t size = ourheadersize - sizeof(struct EthernetHeader);
    dnsreqheader->transaction_id = dnstid++;
    dnsreqheader->flags = 0x1;
    dnsreqheader->question_count = 0x100;
    uint8_t *t4 = (uint8_t*)(dnsreqheader);
    t4[sizeof(struct DNSREQUESTHeader)] = 0;
    int i = 0;
    for(i = 0 ; i < str ; i++){
        t4[sizeof(struct DNSREQUESTHeader)+i+1] = name[i];
    }
    i = 0;
    while( i < (str + 2) ){
        int z = 0;
        for(int t = i+1 ; t < str+2 ; t++){
            uint8_t deze = t4[sizeof(struct DNSREQUESTHeader)+t];
            if(deze==0||deze=='.'){
                break;
            }
            z++;
        }
        t4[sizeof(struct DNSREQUESTHeader)+i] = z;
        i += z + 1;
    }
    t4[sizeof(struct DNSREQUESTHeader)+str+3] = 1;
    t4[sizeof(struct DNSREQUESTHeader)+str+5] = 1;
    
    fillUdpHeader((struct UDPHeader*)&dnsreqheader->udpheader,destmac,size,((uint32_t*)&our_ip)[0],((uint32_t*)&dns_ip)[0],56331,53);
    
    PackageRecievedDescriptor sec;
    sec.buffersize = ourheadersize;
    sec.buffer = dnsreqheader;
    sendEthernetPackage(sec);
    struct DNSREQUESTHeader* de;
    PackageRecievedDescriptor ep;
    uint8_t* targetip = (uint8_t*) malloc(SIZE_OF_IP);
    while(1){
        ep = getEthernetPackage();
        if(ep.buffer==0){
            k_printf("[ETH] IP of %s cannot be found \n",name);
            return targetip;
        }
        de = (struct DNSREQUESTHeader*) ep.buffer;
        if(de->transaction_id==dnsreqheader->transaction_id){
            break;
        }
        ethernet_handle_package(ep);
    }
    if(switch_endian16(de->answer_rr)>0){
        targetip[0] = ((uint8_t*)de + (ep.buffersize-4))[0];
        targetip[1] = ((uint8_t*)de + (ep.buffersize-3))[0];
        targetip[2] = ((uint8_t*)de + (ep.buffersize-2))[0];
        targetip[3] = ((uint8_t*)de + (ep.buffersize-1))[0];
    }
    k_printf("[ETH] IP of %s is %d.%d.%d.%d \n",name,targetip[0],targetip[1],targetip[2],targetip[3]);
    return targetip; 
}

uint32_t ethjmplist[20000];

void setTcpHandler(uint16_t port,uint32_t func){
    ethjmplist[port] = func;
}

TCPMemory tcpmemory[20];
int tcpmemmap;

void create_tcp_session(uint32_t from, uint32_t to, uint16_t from_port, uint16_t to_port, upointer_t func){
    uint32_t sizetype = sizeof(struct TCPHeader);
    struct TCPHeader* tcp1 = (struct TCPHeader*) malloc(sizetype);
    uint8_t* destmac;
    uint8_t* t4 = (uint8_t*)&to;
    uint8_t bckp_from[8];
    memcpy(bckp_from,our_ip,4);
    if(from==0){
        from = (uint32_t)((uint32_t*)bckp_from)[0];
    }

    if(t4[0]==192){
        k_printf("intern zoeken.....\n");
        destmac = getMACFromIp(t4);
    }else{
        k_printf("extern zoeken.....%d \n",t4[0]);
        destmac = getMACFromIp((uint8_t*)&router_ip);
    }
    uint16_t size = sizeof(struct TCPHeader) - sizeof(struct EthernetHeader);
    fillTcpHeader(tcp1,destmac,size,from,to,from_port,to_port,1,0,5,TCP_SYN,0xffd7);

    tcpmemory[tcpmemmap].destmac = destmac;
    tcpmemory[tcpmemmap].from = from;
    tcpmemory[tcpmemmap].to = to;
    tcpmemory[tcpmemmap].port = from_port;
    tcpmemory[tcpmemmap].sequence_number = 1;
    tcpmemory[tcpmemmap].acknowledge_number = 0;
    tcpmemory[tcpmemmap].sendcount = 0;
    tcpmemmap++;

    setTcpHandler(to_port,func);

    PackageRecievedDescriptor sec;
    sec.buffersize = sizetype;
    sec.buffer = tcp1;
    sendEthernetPackage(sec);
}

void send_tcp_package(uint16_t port,upointer_t data,uint16_t length){
    int i = 0 ;
    for(int z = 0 ; z < tcpmemmap ; z++){
        if(tcpmemory[z].port==port){
            i = z;
        }
    }
    uint32_t sizetype = sizeof(struct TCPHeader) + length;
    struct TCPHeader* tcp1 = (struct TCPHeader*) malloc(sizetype*2);
    memset(tcp1,0,sizetype*2);
    uint8_t *early = (uint8_t*) (tcp1 + sizeof(struct TCPHeader) );
    uint8_t *source = (uint8_t*) data;
    int d = 0;
    for(int z = sizeof(struct TCPHeader) ; z < sizetype ; z++){
        ((uint8_t*)tcp1)[z]=source[d++];
    }
    
    uint16_t size = (sizeof(struct TCPHeader) - sizeof(struct EthernetHeader))+length;
    fillTcpHeader(tcp1,tcpmemory[i].destmac,size+1,tcpmemory[i].from,tcpmemory[i].to,port,port,tcpmemory[i].acknowledge_number,tcpmemory[i].sequence_number,5,TCP_PUS | TCP_ACK,0xffd7);

    PackageRecievedDescriptor sec;
    sec.buffersize = sizetype+1;
    sec.buffer = tcp1;
    sendEthernetPackage(sec);

    k_printf("tcp: port:%d data:%s length:%d \n",port,data,length);
}

int ethernet_handle_package(PackageRecievedDescriptor desc){
    struct EthernetHeader *eh = (struct EthernetHeader*) desc.buffer;
    if(eh->type==ETHERNET_TYPE_ARP){
        struct ARPHeader *ah = (struct ARPHeader*) desc.buffer;
        if( ah->operation==0x0100 && memcmp((char*) ah->dest_ip,(char*) &our_ip, SIZE_OF_IP)==0 ){
            k_printf("[ETH] ARP recieved with our IP\n");

            struct ARPHeader* arpie = (struct ARPHeader*)malloc(sizeof(struct ARPHeader));
            uint8_t everyone[SIZE_OF_MAC] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
            fillEthernetHeader((struct EthernetHeader*) &arpie->ethernetheader,everyone,ETHERNET_TYPE_ARP);
            arpie->hardware_type = 0x0100;
            arpie->protocol_type = 0x0008;
            arpie->hardware_address_length = SIZE_OF_MAC;
            arpie->protocol_address_length = SIZE_OF_IP;
            arpie->operation = 0x0200;

            fillMac((uint8_t*)&arpie->source_mac,(uint8_t*)&defaultEthernetDevice.mac);
            fillIP((uint8_t*)&arpie->source_ip,(uint8_t*)&our_ip);

            fillMac((uint8_t*)&arpie->dest_mac,(uint8_t*)ah->source_mac);
            fillIP((uint8_t*)&arpie->dest_ip,(uint8_t*)ah->source_ip);
            
            PackageRecievedDescriptor sec;
            sec.buffersize = sizeof(struct ARPHeader);
            sec.buffer = arpie;

            sendEthernetPackage(sec);
            return 1;
        }else if(ah->operation==0x0100){
            return 1;
        }
    }else if(eh->type==ETHERNET_TYPE_IP4){
        struct IPv4Header* ip = (struct IPv4Header*) eh;
        if(ip->protocol==IPV4_TYPE_UDP){
            struct UDPHeader* udp = (struct UDPHeader*) eh;
            // k_printf("[ETH] UDP package recieved for port %x !\n",switch_endian16(udp->destination_port));
            if(udp->destination_port==switch_endian16(50618)){
                // TFTP, automatic ACK
                struct TFTPAcknowledgeHeader* tftp_old = (struct TFTPAcknowledgeHeader*)eh;
                struct TFTPAcknowledgeHeader* tftp = (struct TFTPAcknowledgeHeader*)malloc(sizeof(struct TFTPAcknowledgeHeader));

                tftp->index = tftp_old->index;
                tftp->type = switch_endian16(4);

                uint16_t packagelength = sizeof(struct UDPHeader) + 4 ;
                fillUdpHeader((struct UDPHeader*)&tftp->header,(uint8_t*)tftp_old->header.ipv4header.ethernetheader.from,packagelength-sizeof(struct EthernetHeader),getOurIpAsLong(),tftp_old->header.ipv4header.source_addr,50618,switch_endian16(tftp_old->header.source_port));
            
                PackageRecievedDescriptor sec;
                sec.buffersize = sizeof(struct TFTPAcknowledgeHeader);
                sec.buffer = tftp;

                sendEthernetPackage(sec);

            }
        }else if(ip->protocol==IPV4_TYPE_TCP){
            struct TCPHeader* tcp = (struct TCPHeader*) eh;
            uint16_t fx = switch_endian16(tcp->flags);
            k_printf("[ETH] TCP package recieved for port %d [flags:%x] %s %s %s %s %s !\n",switch_endian16(tcp->destination_port),fx,fx&TCP_PUS?"PUSH":"",fx&TCP_SYN?"SYN":"",fx&TCP_ACK?"ACK":"",fx&TCP_FIN?"FIN":"",fx&TCP_RES?"RES":"");
            if(((fx & TCP_PUS)||(fx & TCP_SYN)||(fx & TCP_FIN)||(fx & TCP_RES)) && (fx & TCP_ACK)){
                // TCP auto accept ACK SYN
                // k_printf("[ETH] TCP package handled\n");
                uint32_t from = tcp->header.dest_addr; 
                uint32_t to = tcp->header.source_addr; 
                uint16_t from_port = switch_endian16(tcp->destination_port); 
                uint16_t to_port = switch_endian16(tcp->source_port);
                uint32_t sizetype = sizeof(struct TCPHeader);
                struct TCPHeader* tcp1 = (struct TCPHeader*) malloc(sizetype);
                uint8_t* destmac = (uint8_t*)tcp->header.ethernetheader.from;
                uint16_t size = sizeof(struct TCPHeader) - sizeof(struct EthernetHeader);
                uint32_t sid = switch_endian32(tcp->sequence_number);
                if(switch_endian16(tcp->flags) & TCP_PUS){
                    uint32_t tr = (desc.buffersize - ((uint32_t)sizeof(struct TCPHeader)))-4;
                    sid += tr;
                }else if(switch_endian16(tcp->flags) & TCP_SYN){
                    sid++;
                }else if(switch_endian16(tcp->flags) & TCP_FIN){
                    sid++;
                }else if(switch_endian16(tcp->flags) & TCP_RES){
                    sid++;
                }
                fillTcpHeader(tcp1,destmac,size,from,to,from_port,to_port,switch_endian32(tcp->acknowledge_number),sid,5,TCP_ACK,512);

                int i = 0 ;
                for(int z = 0 ; z < tcpmemmap ; z++){
                    if(tcpmemory[z].port==from_port){
                        i = z;
                    }
                }
                tcpmemory[i].sequence_number = sid;
                tcpmemory[i].acknowledge_number = switch_endian32(tcp->acknowledge_number);

                PackageRecievedDescriptor sec;
                sec.buffersize = sizetype;
                sec.buffer = tcp1;
                sendEthernetPackage(sec);

                if(switch_endian16(tcp->flags) & TCP_PUS){
                    upointer_t addr = ((upointer_t)desc.buffer) + sizeof(struct TCPHeader);
                    uint32_t count = desc.buffersize-sizeof(struct TCPHeader);
                    upointer_t func = ethjmplist[switch_endian16(tcp->destination_port)];
                    // k_printf("[ETH] TCP message reieved: size=%x string=%s \n",count,(uint8_t*)addr);
                    if(func){
                        // k_printf("[ETH] function handler is about to get called\n");
                        int (*sendPackage)(uint32_t a,uint32_t b) = (void*)func;
                        sendPackage(addr,count);
                    }else{
                        // k_printf("[ETH] No function handler for this tcpservice!\n");
                    }
                }
                if(switch_endian16(tcp->flags) & TCP_FIN){
                    k_printf("[ETH] Stream is finished!\n");
                }
                if(switch_endian16(tcp->flags) & TCP_RES){
                    k_printf("[ETH] Stream is rejected!\n");
                }
            }
            return 1;
        }else if(ip->protocol==IPV4_TYPE_ICMP){
            struct ICMPHeader *icmp = (struct ICMPHeader*) ip;
            if(icmp->type==8){
                k_printf("[ETH] ICMP ping request found!\n");

                int prefsiz = desc.buffersize - sizeof(struct ICMPHeader);
                struct ICMPHeader *newicmp = (struct ICMPHeader*) malloc(sizeof(struct ICMPHeader) + prefsiz );
                uint16_t size = (sizeof(struct ICMPHeader) + prefsiz) - sizeof(struct EthernetHeader);
                fillIpv4Header((struct IPv4Header*)&newicmp->ipv4header,(uint8_t*)icmp->ipv4header.ethernetheader.from,size,IPV4_TYPE_ICMP,icmp->ipv4header.dest_addr,icmp->ipv4header.source_addr);
                uint8_t *tty = (uint8_t *)icmp;
                uint8_t *tt0 = (uint8_t *)newicmp;
                for(unsigned int i = 0 ; i < ((sizeof(struct ICMPHeader) + prefsiz) - sizeof(struct IPv4Header)) ; i++){
                    tt0[sizeof(struct IPv4Header)+i] = tty[sizeof(struct IPv4Header)+i];
                }
                newicmp->type = 0;

                uint16_t tu = switch_endian16(~icmp->checksum);
                tu -= 0x800;
                newicmp->checksum = switch_endian16(~tu);
                
                PackageRecievedDescriptor sec;
                sec.buffersize = sizeof(struct ICMPHeader) + prefsiz;
                sec.buffer = newicmp;
                sendEthernetPackage(sec);
                return 1;
            }
        }else{
            k_printf("eth: unknown ipv4 protocol: %d \n",ip->protocol);
        }
    }
    return 0;
}

uint32_t getOurIpAsLong(){
    return ((uint32_t*)&our_ip)[0];
}

void exsend(upointer_t addr,uint32_t count){
    k_printf("Recieved message: ");
    for(uint32_t i = 0 ; i < count ; i++){
        k_printf("%c",((uint8_t*)addr)[i]);
    }
    k_printf("\n");
}

void initialise_ethernet(){
    k_printf("[ETH] Ethernet module reached!\n");
    EthernetDevice ed = getDefaultEthernetDevice();
    if(ed.is_enabled){
        k_printf("[ETH] There is a ethernet device present on the system!\n");
        k_printf("[ETH] Asking DHCP server for our address....\n");

        uint8_t *dhcpid = getIpAddressFromDHCPServer();
        if(dhcpid){
            fillIP((uint8_t*)&our_ip,dhcpid);
            k_printf("[ETH] DHCP is present\n");
        }else{
            k_printf("[ETH] No DHCP server present here, using static address\n");
            uint8_t dinges[SIZE_OF_IP] = {192,168,178,15};   
            fillIP((uint8_t*)&our_ip,(uint8_t*)&dinges);
        }

        k_printf("[ETH] Our     IP is %d.%d.%d.%d \n",our_ip[0],our_ip[1],our_ip[2],our_ip[3]);
        k_printf("[ETH] Gateway IP is %d.%d.%d.%d \n",router_ip[0],router_ip[1],router_ip[2],router_ip[3]);
        k_printf("[ETH] DNS     IP is %d.%d.%d.%d \n",dns_ip[0],dns_ip[1],dns_ip[2],dns_ip[3]);
        k_printf("[ETH] DHCP    IP is %d.%d.%d.%d \n",dhcp_ip[0],dhcp_ip[1],dhcp_ip[2],dhcp_ip[3]);
        // for(;;);

        // if(dns_ip[0]){
        //     uint8_t* srve = getIPFromName("tftp.local");
        //     if(srve[0]){
        //         printf("[ETH] TFTP    IP is %d.%d.%d.%d \n",srve[0],srve[1],srve[2],srve[3]);
        //         debugf("[ETH] TFTP    IP is %d.%d.%d.%d \n",srve[0],srve[1],srve[2],srve[3]);
        //         uint8_t ipfs[SIZE_OF_IP];
        //         ipfs[0] = dhcp_ip[0];
        //         ipfs[1] = dhcp_ip[1];
        //         ipfs[2] = dhcp_ip[2];
        //         ipfs[3] = dhcp_ip[3];
        //         Device *dev = getNextFreeDevice();
        //         dev->arg4 = (uint32_t)&ipfs;
        //         dev->arg5 = (uint32_t)getMACFromIp((uint8_t*)&ipfs);
        //         initialiseTFTP(dev);
        //     }
        // }
    }
}