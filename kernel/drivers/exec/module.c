#include "../../include/exec/module.h"
#include "../../include/pci.h"
#include "../../include/device.h"
#include "../../include/memory.h"
#include "../../include/graphics.h"

void stalk(char *t,...){
    k_printf("call went well!\n%s\n",t);
}

char loadModule(char* path, PCIInfo *pci){
    // request paging
    void *programmem = requestPage();

    // loading file
    if( !readFile(path,programmem) ){
        return 0;
    }

    // casting elfheader
    Elf64_Ehdr* elfheader = (Elf64_Ehdr*) programmem;

    // check elfheader
    if(!(elfheader->e_ident[0]==0x7F && elfheader->e_ident[1]=='E' && elfheader->e_ident[2]=='L' && elfheader->e_ident[3]=='F' && elfheader->e_ident[4]==2 )){
        return 0;
    }

    // check elftype, should be 1 for relocatable (kernelmodule)
    if( elfheader->e_type!=1 ){
        return 0;
    }

    Elf64_Shdr *sections = (Elf64_Shdr*) (programmem + elfheader->e_shoff);
    for(Elf64_Half i = 0 ; i < elfheader->e_shnum ; i++){
        Elf64_Shdr *section = (Elf64_Shdr*) &sections[i];
        if(section->sh_type==4){
            k_printf("SHT_RELA\n");
            // clear_screen(0xFFFF0000);
            Elf64_Shdr *section_target = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[section->sh_info]; // is type 1
            Elf64_Shdr *section_symbol = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[section->sh_link];
            Elf64_Shdr *section_string = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[section_symbol->sh_link];
            // for(int q = 0 ; q < 7 ; q++){
            //     Elf64_Sym *symbol = (Elf64_Sym*) &((Elf64_Sym*)(programmem+section_symbol->sh_offset))[q];
            //     k_printf("symbol%d: info:%x name:%d other:%d shndx:%d size:%d value:%d \n",q,symbol->st_info,symbol->st_name,symbol->st_other,symbol->st_shndx,symbol->st_size,symbol->st_value);
            // }
            for(Elf64_Xword j = 0 ; j < (section->sh_size / section->sh_entsize) ; j++){
                Elf64_Rela* relocation = (Elf64_Rela*)(programmem + section->sh_offset + (sizeof(Elf64_Rela)*j));
                Elf64_Xword sym  = ELF64_R_SYM(relocation->r_info);
                Elf64_Xword type = ELF64_R_TYPE(relocation->r_info);
                Elf64_Sym *symbol = (Elf64_Sym*) &((Elf64_Sym*)(programmem+section_symbol->sh_offset))[sym];
                uint64_t symval64 = symbol->st_value;
                uint64_t symval32 = symbol->st_value;
                k_printf("symbol%d: info:%x name:%d other:%d shndx:%d size:%d value:%d \n",sym,symbol->st_info,symbol->st_name,symbol->st_other,symbol->st_shndx,symbol->st_size,symbol->st_value);
                if( symbol->st_info&0x10 ){
                    char* symbolname = (char*) (programmem + section_string->sh_offset + symbol->st_name);
                    if(strcmp(symbolname,"k_printf",strlen("k_printf"))){
                        symval64 = (uint64_t) k_printf;
                        symval32 = (uint64_t) k_printf;
                    }else{
                        k_printf("cannot find symbol: %s \n",symbolname);
                        return 0;
                    }
                }
                if(type==1){ // R_X86_64_64 1 word64 S + A
                    k_printf("R_X86_64_64 - ");
                    uint64_t *ref = (uint64_t *)((programmem + section_target->sh_offset + relocation->r_addend + relocation->r_offset + symbol->st_value));
                    *ref = ((symval64) + (*ref));
                }else if(type==2){ // R_X86_64_PC32 2 word32 S + A - P
                    uint32_t *ref = (uint32_t *)((programmem + section_target->sh_offset + relocation->r_addend + relocation->r_offset + symbol->st_value ));
                    *ref = ((symval32) + (*ref)- relocation->r_offset);
                    k_printf("R_X86_64_PC32 - ");
                }else{
                    k_printf("unknown relocation type: %d \n",type);
                    return 0;
                }
                k_printf("@ addend: %d offset: %d symbol: %d type: %d \n",relocation->r_addend,relocation->r_offset,sym,type);
            }
        }
        if(section->sh_type==8){
            k_printf("SHT_NOBITS");
            if(section->sh_size && section->sh_flags & SHF_ALLOC){
                void *meminfo = requestPage();
                section->sh_offset = meminfo - programmem;
                k_printf(" newalloc:%x ",section->sh_offset);
            }
        }
        if(section->sh_type==9){
            k_printf("SHT_REL");for(;;);
        }
    }
    uint64_t tv = 0x127040;
	void (*KernelStart)() = ((__attribute__((sysv_abi)) void (*)()) tv );
    KernelStart();
    return 1;
}