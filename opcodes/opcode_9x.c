#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_90(void) {
    // BCC (Branch if Carry Clear)
    cpu.PC++;
    if (!(cpu.SR & FLAG_CARRY)) {
        cpu.PC += (int8_t)readMemory(cpu.PC,1);
        cpu.PC++;
        clkCount++;
    } else {
      cpu.PC++;
    }

}

void OPCODE_91(void) {
    // STA (Indirect),Y
    uint16_t addr = addrIndirect_Y();  // Verwende addrIndirect_Y für die Adressierung
    writeMemory(addr, cpu.A);         // Schreibe Akkumulator an die berechnete Adresse
    cpu.PC++;
}

void OPCODE_94(void) {
    // STY Zero Page,X
    uint16_t addr = addrZeropageX();   // Verwende addrZeropageX für die Adressierung
    writeMemory(addr, cpu.Y);         // Schreibe Y-Register an die berechnete Adresse
    cpu.PC++;
}

void OPCODE_95(void) {
    // STA Zero Page,X
    uint16_t addr = addrZeropageX();   // Verwende addrZeropageX für die Adressierung
    writeMemory(addr, cpu.A);         // Schreibe Akkumulator an die berechnete Adresse
    cpu.PC++;
}

void OPCODE_96(void) {
    // STX Zero Page,Y
    uint16_t addr = addrZeropageY();   // Verwende addrZeropageY für die Adressierung
    writeMemory(addr, cpu.X);         // Schreibe X-Register an die berechnete Adresse
    cpu.PC++;
}

void OPCODE_98(void) {
    // TYA (Transfer Y to A)
    cpu.A = cpu.Y;
    setFlag(FLAG_ZERO, cpu.A == 0);
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_99(void) {
    // STA Absolute,Y
    uint16_t addr = addrAbsoluteY();    // Verwende addrAbsoluteY für die Adressierung
    writeMemory(addr, cpu.A);         // Schreibe Akkumulator an die berechnete Adresse
    cpu.PC++;
}

void OPCODE_9A(void) {
    // TXS (Transfer X to Stack Pointer)
    cpu.SP = cpu.X;
    cpu.PC++;
}

void OPCODE_9D(void) {
    // STA Absolute,X
    uint16_t addr = addrAbsoluteX();    // Verwende addrAbsoluteX für die Adressierung
    writeMemory(addr, cpu.A);         // Schreibe Akkumulator an die berechnete Adresse
    cpu.PC++;
}
