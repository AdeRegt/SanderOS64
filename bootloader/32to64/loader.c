#include <stdint.h>
#include "include/graphics.h"
#include "../../kernel/include/bootstrapping/GRUB/multiboot.h"
#include <elf.h>
#include "../../kernel/include/outint.h"

void __stack_chk_fail(){
    for(;;);
}

void __stack_chk_fail_local(){
    for(;;);
}

upointer_t strlen(uint8_t* message){
    upointer_t res = 0;
    uint8_t this = 1;
    while(this){
        this = message[res++];
    }
    return res;
}

upointer_t strcmp(uint8_t* a, uint8_t* b){
    upointer_t length = strlen(b);
	for (upointer_t i = 0; i < length; i++){
		if (*a != *b) return 0;
        a++;
        b++;
	}
	return 1;
}

void *kernellocation = 0;
uint64_t epoint_kernel;
uint64_t epoint_stub;

void *memcpy(void *dst, const void *src, size_t len){
    char *d = dst;
    const char *s = src;
    for (size_t i=0; i<len; i++) {
        d[i] = s[i];
    }
    return dst;
}

uint64_t loadELFImage(void *addr){
    Elf64_Ehdr* header = (Elf64_Ehdr*) addr;

    // for(Elf64_Half i = 0 ; i < header->e_shnum ; i++){
    //     Elf64_Shdr* shr = (Elf64_Shdr*) (header->e_shoff + addr + (header->e_shentsize*i));
    //     if(shr->sh_flags&2){
    //         for(uint64_t z = 0 ; z < shr->sh_size; z++){
    //             ((uint8_t*)(shr->sh_addr+z))[0] = ((uint8_t*)(shr->sh_offset + addr + z))[0];
    //         }
    //     }
    // }

    Elf64_Phdr* phdrs = (Elf64_Phdr*) (addr + header->e_phoff);

	for (Elf64_Phdr* phdr = phdrs;(char*)phdr < (char*)phdrs + header->e_phnum * header->e_phentsize;phdr = (Elf64_Phdr*)((char*)phdr + header->e_phentsize))
	{
		switch (phdr->p_type){
			case PT_LOAD:
			{
				Elf64_Addr segment = phdr->p_paddr;
                k_printf("Loading segment %x with the size of %d from %x \n",(uint32_t)segment,(uint32_t)phdr->p_filesz,((uint32_t)phdr->p_offset + addr));
                memcpy((void*)(uint32_t)segment,((uint32_t)phdr->p_offset + addr),(uint32_t)phdr->p_filesz);
				break;
			}
		}
	}
    return header->e_entry;
}

void nolongmodelistener(){
    k_printf("No longmode available!\n");
}

void ep_grub(multiboot_info_t *grub, uint32_t magic){
    GraphicsInfo c0;
    GraphicsInfo *c1 = (GraphicsInfo*) &c0;

    c1->BaseAddress = (void*) (uint32_t)grub->framebuffer_addr;
    c1->Width = grub->framebuffer_width;
    c1->Height = grub->framebuffer_height;
    c1->PixelsPerScanLine = c1->Width;
    c1->strategy = 1;

    set_graphics_info(c1);

    initialise_graphics_driver();

    k_printf("Welcome to SanderOS64 chainloader 32bit!\n");
    k_printf("There are %d available mods (size: %d) !\n",grub->mods_count,sizeof(struct multiboot_mod_list));

    struct multiboot_mod_list* modlist = (struct multiboot_mod_list*) ( grub->mods_addr + (16*0) );
    void *addstart = (void*)modlist->mod_start;
    epoint_kernel = loadELFImage(addstart);

    modlist = (struct multiboot_mod_list*) ( grub->mods_addr + (16*1) );
    addstart = (void*)modlist->mod_start;
    epoint_stub = loadELFImage(addstart);
    
}