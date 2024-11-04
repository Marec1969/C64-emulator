#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"


void OPCODE_20(void) {
    // JSR (Jump to Subroutine)
    uint16_t addr = addrAbsolute();               // Verwende addrAbsolute für die Zieladresse
    pushStack16(cpu.PC);                    // Rücksprungadresse (nach JSR) auf den Stack
    cpu.PC = addr;                               // Springe zur Zieladresse
}

void OPCODE_21(void) {
    // AND (Indirect, X) - Logisches UND mit indirekter X-Adressierung
    uint16_t addr = addrIndirectX();             // Verwende addrIndirectX für die Adresse
    AND_A(readMemory(addr,1));                    // Verwende die AND_A-Funktion
    cpu.PC++;    
}

void OPCODE_24(void) {
    // BIT (Zero Page) - Testet bestimmte Bits in der Zero Page
    uint16_t addr = addrZeropage();              // Verwende addrZeropage für die Zero Page Adresse
    uint8_t value = readMemory(addr,1);           // Lese den Wert aus der Zero Page
    setFlag(FLAG_ZERO, (cpu.A & value) == 0);
    setFlag(FLAG_OVERFLOW, value & 0x40);
    setFlag(FLAG_NEGATIVE, value & 0x80);
    cpu.PC++;
}

void OPCODE_25(void) {
    // AND (Zero Page) - Logisches UND auf die Zero Page
    uint16_t addr = addrZeropage();              // Verwende addrZeropage
    AND_A(readMemory(addr,1));                    // Verwende die AND_A-Funktion
    cpu.PC++;
}

void OPCODE_26(void) {
    // ROL (Zero Page) - Linksrotation der Zero Page
    uint16_t addr = addrZeropage();              // Verwende addrZeropage
    uint8_t value = readMemory(addr,1);
    value = ROL(value);                          // Führe ROL-Operation durch
    writeMemory(addr, value);                   // Schreibe den neuen Wert zurück
    cpu.PC++;
}

void OPCODE_28(void) {
    // PLP (Pull Processor Status) - Holen des Prozessorstatus vom Stack
    cpu.SR = (popStack8() | 0x20) & ~0x10;       // Statusregister vom Stack holen // set unused to 1 and break to 0
    cpu.PC++;
}

void OPCODE_29(void) {
    // AND (Immediate) - Logisches UND mit unmittelbarem Wert
    uint8_t value = readMemory(addrImmediate(),1); // Verwende addrImmediate für die Adresse
    AND_A(value);                                 // Verwende die AND_A-Funktion
    cpu.PC++;
}

void OPCODE_2A(void) {
    // ROL (Accumulator) - Linksrotation des Akkumulators
    ROL_A();                                      // Verwende die ROL_A-Funktion
    cpu.PC++;
}

void OPCODE_2C(void) {
    // BIT (Absolute) - Testet Bits an einer absoluten Adresse
    uint16_t addr = addrAbsolute();               // Verwende addrAbsolute
    uint8_t value = readMemory(addr,1);
    setFlag(FLAG_ZERO, (cpu.A & value) == 0);
    setFlag(FLAG_NEGATIVE, value & 0x80);
    setFlag(FLAG_OVERFLOW, value & 0x40);
    cpu.PC++;
}

void OPCODE_2D(void) {
    // AND (Absolute) - Logisches UND mit absoluter Adresse
    uint16_t addr = addrAbsolute();               // Verwende addrAbsolute
    AND_A(readMemory(addr,1));                     // Verwende die AND_A-Funktion
    cpu.PC++;
}

void OPCODE_2E(void) {
    // ROL (Absolute) - Linksrotation an einer absoluten Adresse
    uint16_t addr = addrAbsolute();               // Verwende addrAbsolute
    uint8_t value = readMemory(addr,1);
    value = ROL(value);                          // Führe ROL-Operation durch
    writeMemory(addr, value);                   // Schreibe den neuen Wert zurück
    cpu.PC++;
}