#pragma once 
#include <stdint.h>

#ifndef _pntr_end
    #define _pntr_end
    #ifdef __x86_64
        typedef uint64_t upointer_t;
    #endif

    #ifndef __x86_64
        typedef uint32_t upointer_t;
    #endif
#endif 