#include "../../include/exec/elf.h"
#include "../../include/device.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"
#include "../../include/ports.h"
#include "../../include/idt.h"
#include "../../include/pci.h"
#include "../../include/timer.h"
#include "../../include/paging.h"
#include "../../include/ethernet.h"
#include "../../include/exec/debugger.h"

uint8_t is_elf(void *programmem){
    Elf64_Ehdr* elfheader = (Elf64_Ehdr*) programmem;

    // check elfheader
    if(!(elfheader->e_ident[0]==0x7F && elfheader->e_ident[1]=='E' && elfheader->e_ident[2]=='L' && elfheader->e_ident[3]=='F' && elfheader->e_ident[4]==2 )){
        return 0;
    }
    return 1;
}

upointer_t elf_load_image(void *programmem){
    #ifdef __x86_64
    // casting elfheader
    Elf64_Ehdr* elfheader = (Elf64_Ehdr*) programmem;

    // check elfheader
    if(!(elfheader->e_ident[0]==0x7F && elfheader->e_ident[1]=='E' && elfheader->e_ident[2]=='L' && elfheader->e_ident[3]=='F' && elfheader->e_ident[4]==2 )){
        return 0;
    }

    // check elftype, should be 1 for relocatable (kernelmodule)
    if( elfheader->e_type==1 ){

        upointer_t entrypoint = 0;
        Elf64_Shdr *section_symbol;
        Elf64_Shdr *section_string;

        Elf64_Shdr *sections = (Elf64_Shdr*) (programmem + elfheader->e_shoff);
        for(Elf64_Half i = 0 ; i < elfheader->e_shnum ; i++){
            Elf64_Shdr *section = (Elf64_Shdr*) &sections[i];
            if(section->sh_type==4){
                
                Elf64_Shdr *section_target = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[section->sh_info]; // is type 1
                section_symbol = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[section->sh_link];
                section_string = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[section_symbol->sh_link];
                
                for(Elf64_Xword j = 0 ; j < (section->sh_size / section->sh_entsize) ; j++){

                    Elf64_Rela* relocation = (Elf64_Rela*)(programmem + section->sh_offset + (sizeof(Elf64_Rela)*j));
                    Elf64_Xword sym  = ELF64_R_SYM(relocation->r_info);
                    Elf64_Xword type = ELF64_R_TYPE(relocation->r_info);
                    Elf64_Sym *symbol = (Elf64_Sym*) &((Elf64_Sym*)(programmem+section_symbol->sh_offset))[sym];
                    upointer_t symval = symbol->st_value;

                    Elf64_Shdr *symbol_target = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[symbol->st_shndx];
                    
                    if( symbol->st_info&0x10 ){
                        char* symbolname = (char*) (programmem + section_string->sh_offset + symbol->st_name);
                        if(memcmp(symbolname,"k_printf",strlen("k_printf"))==0 ){
                            symval = (upointer_t) k_printf;
                        }else if(memcmp(symbolname,"inportb",strlen("inportb"))==0 ){
                            symval = (upointer_t) inportb;
                        }else if(memcmp(symbolname,"outportb",strlen("outportb"))==0 ){
                            symval = (upointer_t) outportb;
                        }else if(memcmp(symbolname,"inportw",strlen("inportw"))==0 ){
                            symval = (upointer_t) inportw;
                        }else if(memcmp(symbolname,"outportw",strlen("outportw"))==0 ){
                            symval = (upointer_t) outportw;
                        }else if(memcmp(symbolname,"inportl",strlen("inportl"))==0 ){
                            symval = (upointer_t) inportl;
                        }else if(memcmp(symbolname,"outportl",strlen("outportl"))==0 ){
                            symval = (upointer_t) outportl;
                        }else if(memcmp(symbolname,"setInterrupt",strlen("setInterrupt"))==0 ){
                            symval = (upointer_t) setInterrupt;
                        }else if(memcmp(symbolname,"memset",strlen("memset"))==0 ){
                            symval = (upointer_t) memset;
                        }else if(memcmp(symbolname,"getBARaddress",strlen("getBARaddress"))==0 ){
                            symval = (upointer_t) getBARaddress;
                        }else if(memcmp(symbolname,"sleep",strlen("sleep"))==0 ){
                            symval = (upointer_t) sleep;
                        }else if(memcmp(symbolname,"requestPage",strlen("requestPage"))==0 ){
                            symval = (upointer_t) requestPage;
                        }else if(memcmp(symbolname,"registerHIDDevice",strlen("registerHIDDevice"))==0 ){
                            symval = (upointer_t) registerHIDDevice;
                        }else if(memcmp(symbolname,"ethernet_set_link_status",strlen("ethernet_set_link_status"))==0 ){
                            symval = (upointer_t) ethernet_set_link_status;
                        }else if(memcmp(symbolname,"ethernet_handle_package",strlen("ethernet_handle_package"))==0 ){
                            symval = (upointer_t) ethernet_handle_package;
                        }else if(memcmp(symbolname,"register_ethernet_device",strlen("register_ethernet_device"))==0 ){
                            symval = (upointer_t) register_ethernet_device;
                        }else{
                            int fnd = 0;
                            for(Elf64_Xword d = 0 ; d < (section_symbol->sh_size/section_symbol->sh_entsize) ; d++){
                                Elf64_Sym *symbol2 = (Elf64_Sym*) &((Elf64_Sym*)(programmem+section_symbol->sh_offset))[d];
                                Elf64_Shdr *symbol_target2 = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[symbol2->st_shndx];
                                char* symbolname2 = (char*) (programmem + section_string->sh_offset + symbol2->st_name);
                                if(memcmp(symbolname,symbolname2,strlen(symbolname2))==0 && symbol2->st_shndx ){
                                    symval = (upointer_t)(programmem + symbol_target2->sh_offset + symbol2->st_value);
                                    fnd = 1;
                                }
                            }
                            if(fnd==0){
                                k_printf("cannot find symbol: %s \n",symbolname);
                                return 0;
                            }
                        }
                    }

                    upointer_t S = symval;
                    upointer_t A = (upointer_t)(relocation->r_addend + (symbol->st_info&0x10?0:(programmem + symbol_target->sh_offset)));
                    upointer_t P = (upointer_t)programmem + section_target->sh_offset + relocation->r_offset;

                    if(type==1){ 
                        // R_X86_64_64 1 word64 S + A
                        // k_printf("R_X86_64_64 - ");
                        upointer_t *ref = (upointer_t *)((programmem + section_target->sh_offset + relocation->r_offset));
                        upointer_t c = S + A;
                        *ref = c;
                    }else if(type==2){ 
                        // R_X86_64_PC32 2 word32 S + A - P
                        // k_printf("R_X86_64_PC32 - ");
                        uint32_t *ref = (uint32_t *)((programmem + section_target->sh_offset + relocation->r_offset));
                        // uint32_t c = S + A - P;
                        // *ref = c;
                        uint32_t c = (uint32_t)((upointer_t)programmem + section_target->sh_offset + relocation->r_addend);
                        *ref = c;
                        // k_printf("S=%x A=%x P=%x \n",S,A,P);
                        // k_printf("addend: %x info:%x offset:%x res:%x\n",relocation->r_addend,relocation->r_info,relocation->r_offset,c);
                    }else{
                        k_printf("unknown relocation type: %d \n",type);
                        return 0;
                    }
                    
                }
            }

            if(section->sh_type==8){
                if(section->sh_size && section->sh_flags & SHF_ALLOC){
                    void *meminfo = requestPage();
                    section->sh_offset = meminfo - programmem;
                }
            }

            if(section->sh_type==9){
                k_printf("SHT_REL");
                return 0;
            }

        }
        
        // symbol lookup....
        for(Elf64_Xword i = 0 ; i < (section_symbol->sh_size/section_symbol->sh_entsize) ; i++){
            Elf64_Sym *symbol = (Elf64_Sym*) &((Elf64_Sym*)(programmem+section_symbol->sh_offset))[i];
            Elf64_Shdr *symbol_target = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[symbol->st_shndx];
            char* symbolname = (char*) (programmem + section_string->sh_offset + symbol->st_name);
            if(strcmp(symbolname,"driver_start",strlen("driver_start"))){
                elfheader->e_entry = (Elf64_Addr)(programmem + symbol_target->sh_offset + symbol->st_value);
                // clear_screen(0xFFFFFF);
                // debugger_interpetate_next_x_instructions((void*)elfheader->e_entry,48);
            }
        }
    }else if( elfheader->e_type==2 ){
        Elf64_Phdr* phdrs = (Elf64_Phdr*) ( programmem + elfheader->e_phoff );
        for(Elf64_Half i = 0 ; i<elfheader->e_phnum ; i++){
            Elf64_Phdr* ch = (Elf64_Phdr*) (programmem + (elfheader->e_phentsize * elfheader->e_phnum));
            if(ch->p_type==1){
                memcpy((void*)ch->p_paddr, programmem + ch->p_offset , ch->p_memsz);
            }
        }
    }else{
        return 0;
    }
    k_printf("Programs EP at %x \n",elfheader->e_entry);
    return elfheader->e_entry;
    #endif 
    #ifndef __x86_64
    return 0;
    #endif 
}