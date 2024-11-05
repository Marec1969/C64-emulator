#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_D0(void) {
    // BNE (Branch if Not Equal)
    cpu.PC++;
    if (!(cpu.SR & FLAG_ZERO)) {
        cpu.PC += (int8_t)readMemory(cpu.PC, 1);
        cpu.PC++;
        clkCount++;
    } else {
        cpu.PC++;
    }
}

void OPCODE_D1(void) {
    // CMP (Indirect),Y
    uint16_t addr = addrIndirect_Y();  // Verwende addrIndirect_Y für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == value);
    setFlag(FLAG_NEGATIVE, cpu.A < value);
    setFlag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_D5(void) {
    // CMP Zero Page,X
    uint16_t addr = addrZeropageX();  // Verwende addrZeropageX für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == value);
    setFlag(FLAG_NEGATIVE, cpu.A < value);
    setFlag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_D6(void) {
    // DEC Zero Page,X
    uint16_t addr = addrZeropageX();  // Verwende addrZeropageX für die Adressierung
    uint8_t value = readMemory(addr, 1);
    value = DEC(value);
    writeMemory(addr, value);
    cpu.PC++;
}

void OPCODE_D8(void) {
    // CLD (Clear Decimal Flag)
    cpu.SR &= ~FLAG_DECIMAL;
    cpu.PC++;
}

void OPCODE_D9(void) {
    // CMP Absolute,Y
    uint16_t addr = addrAbsoluteY();  // Verwende addrAbsoluteY für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == value);
    setFlag(FLAG_NEGATIVE, cpu.A < value);
    setFlag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_DD(void) {
    // CMP Absolute,X
    uint16_t addr = addrAbsoluteX();  // Verwende addrAbsoluteX für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == value);
    setFlag(FLAG_NEGATIVE, cpu.A < value);
    setFlag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_DE(void) {
    // DEC Absolute,X
    uint16_t addr = addrAbsoluteX();  // Verwende addrAbsoluteX für die Adressierung
    uint8_t value = readMemory(addr, 1);
    value = DEC(value);
    writeMemory(addr, value);
    cpu.PC++;
}
