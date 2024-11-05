#include "sid.h"

#include <stdint.h>
#include <stdio.h>

sid_t sidRegister;

uint8_t sidRead(uint16_t addr) {
    uint8_t* ptr;

    ptr = (uint8_t*)&sidRegister + (addr - SID_START_ADDR);
    return *ptr;
}

void sidWrite(uint16_t addr, uint8_t value) {
    uint8_t* ptr;
    ptr = (uint8_t*)&sidRegister + (addr - SID_START_ADDR);
    *ptr = value;
}
