#include <stdint.h>
#include <SanderOS.h>

uint32_t time(void *t){
    hang(__FUNCTION__);
    return 1;
}
