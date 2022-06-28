#include "../../include/exec/elf.h"
#include "../../include/graphics.h"
#include "../../include/memory.h"

uint64_t elf_load_image(void *programmem){
    // casting elfheader
    Elf64_Ehdr* elfheader = (Elf64_Ehdr*) programmem;

    // check elfheader
    if(!(elfheader->e_ident[0]==0x7F && elfheader->e_ident[1]=='E' && elfheader->e_ident[2]=='L' && elfheader->e_ident[3]=='F' && elfheader->e_ident[4]==2 )){
        return 0;
    }

    // check elftype, should be 1 for relocatable (kernelmodule)
    if( elfheader->e_type==1 ){

        uint64_t entrypoint = 0;
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
                    uint64_t symval = symbol->st_value;

                    Elf64_Shdr *symbol_target = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[symbol->st_shndx];
                    
                    if( symbol->st_info&0x10 ){
                        char* symbolname = (char*) (programmem + section_string->sh_offset + symbol->st_name);
                        if(strcmp(symbolname,"k_printf",strlen("k_printf"))){
                            symval = (uint64_t) k_printf;
                        }else{
                            k_printf("cannot find symbol: %s \n",symbolname);
                            return 0;
                        }
                    }

                    uint64_t S = symval;
                    uint64_t A = (uint64_t)(relocation->r_addend + (symbol->st_info&0x10?0:(programmem + symbol_target->sh_offset)));
                    uint64_t P = 0;

                    if(type==1){ 
                        // R_X86_64_64 1 word64 S + A
                        // k_printf("R_X86_64_64 - ");
                        uint64_t *ref = (uint64_t *)((programmem + section_target->sh_offset + relocation->r_offset));
                        *ref = S + A;
                    }else if(type==2){ 
                        // R_X86_64_PC32 2 word32 S + A - P
                        // k_printf("R_X86_64_PC32 - ");
                        uint32_t *ref = (uint32_t *)((programmem + section_target->sh_offset + relocation->r_offset));
                        *ref = S + A - P;
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
                    k_printf(" newalloc:%x ",section->sh_offset);
                }
            }

            if(section->sh_type==9){
                k_printf("SHT_REL");for(;;);
            }

        }
        // symbol lookup....
        for(Elf64_Xword i = 0 ; i < (section_symbol->sh_size/section_symbol->sh_entsize) ; i++){
            Elf64_Sym *symbol = (Elf64_Sym*) &((Elf64_Sym*)(programmem+section_symbol->sh_offset))[i];
            Elf64_Shdr *symbol_target = (Elf64_Shdr*) &((Elf64_Shdr*)(programmem + elfheader->e_shoff))[symbol->st_shndx];
            char* symbolname = (char*) (programmem + section_string->sh_offset + symbol->st_name);
            if(strcmp(symbolname,"driver_start",strlen("driver_start"))){
                elfheader->e_entry = (Elf64_Addr)(programmem + symbol_target->sh_offset + symbol->st_value);
            }
        }
    }else{
        return 0;
    }

    return elfheader->e_entry;
}