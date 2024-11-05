
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_10(void) {
    // BPL (Branch if Positive)
    cpu.PC++;                                     // Bewege zum nächsten Byte (Offset lesen)
    if (!(cpu.SR & FLAG_NEGATIVE)) {              // Überprüfen, ob das Negative-Flag gesetzt ist
        cpu.PC += (int8_t)readMemory(cpu.PC, 1);  // PC um den Offset erhöhen
        cpu.PC++;
        clkCount++;
    } else {
        cpu.PC++;
    }
}

void OPCODE_11(void) {
    // ORA (Indirect),Y
    uint16_t addr = addrIndirect_Y();
    ORA_A(readMemory(addr, 1));
    cpu.PC++;
}

void OPCODE_15(void) {
    // ORA Zero Page,cpu.X
    uint8_t addr = addrZeropageX();
    ORA_A(readMemory(addr, 1));
    cpu.PC++;
}

void OPCODE_16(void) {
    // ASL Zero Page,cpu.X
    uint8_t addr = addrZeropageX();
    uint8_t value = readMemory(addr, 1);
    value = ASL(value);
    writeMemory(addr, value);
    cpu.PC++;
}

void OPCODE_18(void) {
    setFlag(FLAG_CARRY, 0);  // CLC (Clear Carry Flag)
    cpu.PC++;
}

void OPCODE_19(void) {
    // SBC Absolute,Y - Subtract with Carry
    uint16_t addr = addrAbsoluteY();
    ORA_A(readMemory(addr, 1));
    cpu.PC++;
}

void OPCODE_1D(void) {
    // ORA Absolute,cpu.X
    uint16_t addr = addrAbsoluteX();
    ORA_A(readMemory(addr, 1));
    cpu.PC++;
}

void OPCODE_1E(void) {
    // ASL Absolute,cpu.X
    uint16_t addr = addrAbsoluteX();
    uint8_t value = readMemory(addr, 1);
    value = ASL(value);
    writeMemory(addr, value);
    cpu.PC++;
}
