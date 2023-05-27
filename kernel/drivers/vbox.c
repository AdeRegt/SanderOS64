#include "../include/vbox.h"
#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/memory.h"
#include "../include/paging.h"
#include "../include/pci.h"
#include "../include/timer.h"

// https://wiki.osdev.org/VirtualBox_Guest_Additions
// http://download.virtualbox.org/virtualbox/SDKRef.pdf

unsigned long vbox_base_addr;

VMMDevHGCMRequestHeader* vbox_offer_package(void* package){
    outportl(vbox_base_addr, (uint32_t) (upointer_t)package);
    return (VMMDevHGCMRequestHeader*)package;
}

VMMDevHGCMConnect *vbox_client_connect(char* type){
    VMMDevHGCMConnect* req = (VMMDevHGCMConnect*) requestPage();
    memset(req,0,sizeof(VMMDevHGCMConnect));
    req->header.size = sizeof(VMMDevHGCMConnect);
    req->header.version = 0x10001;
    req->header.type = 60;
    req->type = 2;
    for(int i = 0 ; i < strlen(type)+1 ; i++){
        req->location[i] = type[i];
    }
    vbox_offer_package(req);
    sleep(10);
    volatile VMMDevHGCMConnect* fr = (VMMDevHGCMConnect*) req;
    k_printf(":: clientId:%x rc:%x  \n",fr->clientId,fr->header.rc);
    return req;
}

void *vbox_client_introduce(){
    vbox_guest_info *req = (vbox_guest_info*) requestPage();
    memset(req,0,sizeof(vbox_guest_info));
    req->size = sizeof(vbox_guest_info);
    req->version = 0x10001;
    req->type = 50;
    req->version2 = 0x00010003;
    vbox_offer_package(req);
}

void vbox_driver_start(int bus,int slot,int function){
    vbox_base_addr = getBARaddress(bus,slot,function,0x10) & 0xFFFFFFFC;
    k_printf("vbox: driver found, BAR0 has the value of %x !\n",vbox_base_addr);
    vbox_client_introduce();
    vbox_client_connect("VBoxSharedFolders");
}