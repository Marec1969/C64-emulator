#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_F0(void) {
    // BEQ (Branch if Equal)
    cpu.PC++;
    if (cpu.SR & FLAG_ZERO) {
        cpu.PC += (int8_t)readMemory(cpu.PC, 1);
        cpu.PC++;
        clkCount++;
    } else {
        cpu.PC++;
    }
}

void OPCODE_F1(void) {
    // SBC (Indirect),Y
    uint16_t addr = addrIndirect_Y();  // Verwende addrIndirect_Y für die Adressierung
    uint8_t value = readMemory(addr, 1);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_F5(void) {
    // SBC Zero Page,X
    uint8_t addr = addrZeropageX();  // Verwende addrZeropage_X für die Adressierung
    uint8_t value = readMemory(addr, 1);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_F6(void) {
    // INC Zero Page,X
    uint8_t addr = addrZeropageX();  // Verwende addrZeropage_X für die Adressierung
    uint8_t value = readMemory(addr, 1);
    value = INC(value);
    writeMemory(addr, value);
    cpu.PC++;
}

void OPCODE_F8(void) {
    // SED (Set Decimal Flag)
    cpu.SR |= FLAG_DECIMAL;
    cpu.PC++;
}

void OPCODE_F9(void) {
    // SBC Absolute,Y
    uint16_t addr = addrAbsoluteY();
    uint8_t value = readMemory(addr, 1);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_FD(void) {
    // SBC Absolute,X
    uint16_t addr = addrAbsoluteX();
    uint8_t value = readMemory(addr, 1);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_FE(void) {
    // INC Absolute,X
    uint16_t addr = addrAbsoluteX();
    uint8_t value = readMemory(addr, 1);
    value = INC(value);
    writeMemory(addr, value);
    cpu.PC++;
}
