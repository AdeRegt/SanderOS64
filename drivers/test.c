#include "driver.h"

int driver_start(PCIInfo *pi){
    k_printf("BUS=%x FUNC=%x SLOT=%x \n",pi->bus,pi->function,pi->slot);
}