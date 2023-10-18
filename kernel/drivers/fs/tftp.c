#include "../../include/fs/tftp.h"
#include "../../include/ethernet.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"

char* load_tftp_file(char* path){
    // are we there?
    if(ethernet_is_enabled()==0){
        k_printf("tftp: ethernet is not enabled!\n");
        return 0;
    }
    // we load tftp from a static address...
    uint8_t *ipaddr = "    ";
    ipaddr[0] = 192;
    ipaddr[1] = 168;
    ipaddr[2] = 2;
    ipaddr[3] = 4;
    uint8_t* mac = getMACFromIp(ipaddr);
    if(mac==0){
        k_printf("tftp: cannot find MAC by IP address\n");
        return 0;
    }
    // ok, now lets create a command to request the tftp 
    TFTPRequestFile* tftp_request = (TFTPRequestFile*)requestPage();
    tftp_request->operation = switch_endian16(1);
    int stok = 0;
    for(int i = 0 ; i < strlen(path)-1 ; i++){
        tftp_request->data[stok++] = path[i];
    }
    tftp_request->data[stok++] = 0;
    tftp_request->data[stok++] = 'o';
    tftp_request->data[stok++] = 'c';
    tftp_request->data[stok++] = 't';
    tftp_request->data[stok++] = 'e';
    tftp_request->data[stok++] = 't';
    tftp_request->data[stok++] = 0;
    tftp_request->data[stok++] = 't';
    tftp_request->data[stok++] = 's';
    tftp_request->data[stok++] = 'i';
    tftp_request->data[stok++] = 'z';
    tftp_request->data[stok++] = 'e';
    tftp_request->data[stok++] = 0;
    tftp_request->data[stok++] = '0';
    tftp_request->data[stok++] = 0;

    unsigned short size = (sizeof(TFTPRequestFile) + (stok-50) ) - sizeof(struct EthernetHeader);
    fillUdpHeader((struct UDPHeader*)&tftp_request->udpheader,mac,size,getOurIpAsLong(),((uint32_t*)ipaddr)[0],TFTP_DEFAULT_PORT,TFTP_DEFAULT_PORT);

    PackageRecievedDescriptor sec;
    sec.buffersize = sizeof(TFTPRequestFile)+ (stok-50);
    sec.high_buf = 0;
    sec.low_buf = (unsigned long)tftp_request;
    int res_fs = sendEthernetPackage(sec); // send package
    if(res_fs==0){
        k_printf("tftp: failed to send package request\n");
        return 0;
    }
    
    // now we need to get data back...
    PackageRecievedDescriptor prd;
    struct UDPHeader* udp;
    while(1){
        prd = getEthernetPackage(); 
        if(prd.low_buf==0){
            return 0;
        }
        struct EthernetHeader *eh = (struct EthernetHeader*) prd.low_buf;
        struct IPv4Header* ip = (struct IPv4Header*) eh;
        if(eh->type==ETHERNET_TYPE_IP4 && ip->protocol==IPV4_TYPE_UDP){
            udp = (struct UDPHeader*) eh;
            if(switch_endian16(udp->destination_port)==TFTP_DEFAULT_PORT){
                break;
            }
        }
    }
    // now we recieve data...
    TFTPOptionFile* tftpresponse = (TFTPOptionFile*) udp;
    if(switch_endian16(tftpresponse->operation)!=6){
        k_printf("tftp: unexpected operation\n");
        return 0;
    }
    char* expectedsize = (char*)&tftpresponse->value;

    TFTPResponseFile* tftp_response = (TFTPResponseFile*)requestPage();
    tftp_response->operation = switch_endian16(4);
    tftp_response->block = 0;

    size = sizeof(TFTPResponseFile) - sizeof(struct EthernetHeader);
    fillUdpHeader((struct UDPHeader*)&tftp_response->udpheader,mac,size,getOurIpAsLong(),((uint32_t*)ipaddr)[0],TFTP_DEFAULT_PORT,switch_endian16(tftpresponse->udpheader.source_port));
    sec.buffersize = sizeof(TFTPResponseFile);
    sec.high_buf = 0;
    sec.low_buf = (unsigned long)tftp_response;
    res_fs = sendEthernetPackage(sec); // send package
    if(res_fs==0){
        k_printf("tftp: failed to send package acknowledge\n");
        return 0;
    }

    void *createdbuffer = requestPage();
    int foundsize = 0;

    onceagain:
    while(1){
        prd = getEthernetPackage(); 
        if(prd.low_buf==0){
            return 0;
        }
        struct EthernetHeader *eh = (struct EthernetHeader*) prd.low_buf;
        struct IPv4Header* ip = (struct IPv4Header*) eh;
        if(eh->type==ETHERNET_TYPE_IP4 && ip->protocol==IPV4_TYPE_UDP){
            udp = (struct UDPHeader*) eh;
            if(switch_endian16(udp->destination_port)==TFTP_DEFAULT_PORT){
                break;
            }
        }
    }
    TFTPResponseFile* tftpresponse2 = (TFTPResponseFile*) udp;

    freePage(tftp_response);
    tftp_response = (TFTPResponseFile*)requestPage();
    tftp_response->operation = switch_endian16(4);
    tftp_response->block = tftpresponse2->block;

    size = sizeof(TFTPResponseFile) - sizeof(struct EthernetHeader);
    fillUdpHeader((struct UDPHeader*)&tftp_response->udpheader,mac,size,getOurIpAsLong(),((uint32_t*)ipaddr)[0],TFTP_DEFAULT_PORT,switch_endian16(tftpresponse->udpheader.source_port));
    sec.buffersize = sizeof(TFTPResponseFile);
    sec.high_buf = 0;
    sec.low_buf = (unsigned long)tftp_response;
    res_fs = sendEthernetPackage(sec); // send package
    if(res_fs==0){
        k_printf("tftp: failed to send package acknowledge\n");
        return 0;
    }

    int sizecopied = ( prd.buffersize - sizeof(TFTPResponseFile) ) - 4;
    char* tftpdataresponse = (char*) udp + sizeof(TFTPResponseFile);
    memcpy(createdbuffer + foundsize,tftpdataresponse,sizecopied);
    foundsize += sizecopied;
    char *convertednumber = convert(foundsize,10);
    if(memcmp(expectedsize,convertednumber,strlen(expectedsize))!=0){
        goto onceagain;
    }

    freePage(tftp_request);
    freePage(tftp_response);
    
    return createdbuffer;
}