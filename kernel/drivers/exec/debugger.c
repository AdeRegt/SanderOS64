#include "../../include/exec/debugger.h"
#include "../../include/graphics.h"

int debugger_interpetate_instruction(void* location){
    int size = 0;
    uint8_t *symbol = (uint8_t*) location;
    uint8_t cs = symbol[size++];
    k_printf("%x: ",location);
    if(cs==0x00){
        symbol[size++];
        symbol[size++];
    }else if(cs==0x0f){
        cs = symbol[size++];
        if(cs==0xb6){
            cs = symbol[size++];
            if(cs==0xc0){
                k_printf("movzbl %%al,%%eax");
            }else{
                k_printf("0x0f 0xb6 :: unknown instruction: %x \n",cs);for(;;);
            }
        }else{
            k_printf("0x0f :: unknown instruction: %x \n",cs);for(;;);
        }
    }else if(cs==0x3C){
        cs = symbol[size++];
        k_printf("cmp $%x,%%al",cs);
    }else if(cs==0x55){
        k_printf("push %%rbp ");
    }else if(cs==0x48){
        cs = symbol[size++];
        if(cs==0x89){
            cs = symbol[size++];
            if(cs==0xE5){
                k_printf("mov %%rsp,%%rbp");
            }else if(cs==0x7d){
                cs = symbol[size++];
                k_printf("mov %%rdi,-%x(%%rbp)",cs ^ 0xF0);
            }else if(cs==0xc6){
                k_printf("mov %%rax,%%rsi");
            }else if(cs==0xc7){
                k_printf("mov %%rax,%%rdi");
            }else{
                k_printf("cs 0x48 0x89 :: unknown opcode: %x \n",cs);for(;;);
            }
        }else if(cs==0x83){
            uint8_t cs1 = symbol[size++];
            uint8_t cs2 = symbol[size++];
            k_printf("sub $%x,",cs2);
            if(cs1==0xec){
                k_printf("%%rsp");
            }else{
                k_printf("cs 0x48 0x83 :: unnown opcode: %x \n",cs);for(;;);
            }
        }else if(cs==0xB8){
            uint8_t cs1 = symbol[size++];
            uint8_t cs2 = symbol[size++];
            uint8_t cs3 = symbol[size++];
            uint8_t cs4 = symbol[size++];
            uint8_t cs5 = symbol[size++];
            uint64_t *csx = (uint64_t*) (location+2);
            k_printf("movabs $%x,%%rax",csx[0]);
        }else if(cs==0xBA){
            uint8_t cs1 = symbol[size++];
            uint8_t cs2 = symbol[size++];
            uint8_t cs3 = symbol[size++];
            uint8_t cs4 = symbol[size++];
            uint8_t cs5 = symbol[size++];
            uint64_t *csx = (uint64_t*) (location+2);
            k_printf("movabs $%x,%%rdx",csx[0]);
        }else{
            k_printf("cs 0x48 :: unknown opcode: %x \n",cs);for(;;);
        }
    }else if(cs==0x75){
        k_printf("jne %x",(location+1)-(symbol[size++]^0xFF));
    }else if(cs==0x83){
        cs = symbol[size++];
        if(cs==0xe0){
            cs = symbol[size++];
            k_printf("and $%x,%%eax",cs);
        }else{
            k_printf("0x83 :: unknown opcode: %x \n",cs);
        }
    }else if(cs==0x85){
        cs = symbol[size++];
        if(cs==0xc0){
            k_printf("test %%eax,%%eax");
        }else{
            k_printf("0x85 :: unknown opcode: %x \n",cs);for(;;);
        }
    }else if(cs==0x90){
        k_printf("nop");
    }else if(cs==0xb8){
        uint8_t cs1 = symbol[size++];
        uint8_t cs2 = symbol[size++];
        uint8_t cs3 = symbol[size++];
        uint8_t cs4 = symbol[size++];
        uint32_t *csx = (uint32_t*) (location+1);
        k_printf("mov $%x,%%eax",csx[0]);
    }else if(cs==0xbe){
        uint8_t cs1 = symbol[size++];
        uint8_t cs2 = symbol[size++];
        uint8_t cs3 = symbol[size++];
        uint8_t cs4 = symbol[size++];
        uint32_t *csx = (uint32_t*) (location+1);
        k_printf("mov $%x,%%esi",csx[0]);
    }else if(cs==0xbf){
        uint8_t cs1 = symbol[size++];
        uint8_t cs2 = symbol[size++];
        uint8_t cs3 = symbol[size++];
        uint8_t cs4 = symbol[size++];
        uint32_t *csx = (uint32_t*) (location+1);
        k_printf("mov $%x,%%edi",csx[0]);
    }else if(cs==0xc3){
        k_printf("retq");
    }else if(cs==0xc9){
        k_printf("leaveq");
    }else if(cs==0xff){
        cs = symbol[size++];
        if(cs==0xd2){
            k_printf("callq *%%rdx");
        }else if(cs==0xd0){
            k_printf("callq *%%rax");
        }else{
            k_printf("0xff :: unknown instruction: %x ",cs);for(;;);    
        }
    }else{
        k_printf("unknown instruction: %x ",cs);for(;;);
    }
    return size;
}

int debugger_interpetate_next_x_instructions(void *location,uint64_t times){
    uint64_t pointer = (uint64_t)location;
    for(uint64_t i = 0 ; i < times ; i++){
        pointer += debugger_interpetate_instruction((void*)pointer);
        if(i%2){
            k_printf("\n");
        }else{ 
            k_printf(" ::: ");
        }
    }
}