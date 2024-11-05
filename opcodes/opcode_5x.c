#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"
#include "trace.h"

void OPCODE_50(void) {
    // BVC (Branch if Overflow Clear)
    cpu.PC++;  // Inkrementiere PC
    if (!(cpu.SR & FLAG_OVERFLOW)) {
        cpu.PC += (int8_t)readMemory(cpu.PC, 1);  // Berechne den Offset für den Sprung
        cpu.PC++;
        clkCount++;
    } else {
        cpu.PC++;
    }
}

void OPCODE_51(void) {
    // EOR (Indirect),Y
    uint16_t addr = addrIndirect_Y();  // Lese die indirekte Adresse und addiere Y
    EOR_A(readMemory(addr, 1));        // XOR mit dem Wert von der berechneten Adresse
    cpu.PC++;
}

void OPCODE_55(void) {
    // EOR Zero Page,X
    uint16_t addr = addrZeropageX();  // Berechne Zero-Page-Adresse mit X-Offset
    EOR_A(readMemory(addr, 1));       // Führe bitweises XOR durch
    cpu.PC++;
}

void OPCODE_56(void) {
    // LSR Zero Page,X
    uint16_t addr = addrZeropageX();      // Berechne Zero-Page-Adresse mit X-Offset
    uint8_t value = readMemory(addr, 1);  // Lese den Wert
    value = LSR(value);                   // Führe logisches Shift-Right durch
    writeMemory(addr, value);             // Schreibe neuen Wert zurück
    cpu.PC++;
}

void OPCODE_58(void) {
    // CLI (Set Interrupt Disable)
    cpu.SR &= ~FLAG_INTERRUPT;  // resetze Interrupt-Disable-Flag
    cpu.PC++;
}

void OPCODE_59(void) {
    // EOR Absolute,Y
    uint16_t addr = addrAbsoluteY();  // Lese die absolute Adresse und addiere X
    EOR_A(readMemory(addr, 1));       // Führe bitweises XOR durch
    cpu.PC++;
}

void OPCODE_5D(void) {
    // EOR Absolute,X
    uint16_t addr = addrAbsoluteX();  // Lese die absolute Adresse und addiere X
    EOR_A(readMemory(addr, 1));       // Führe bitweises XOR durch
    cpu.PC++;
}

void OPCODE_5E(void) {
    // LSR Absolute,X
    uint16_t addr = addrAbsoluteX();      // Lese die absolute Adresse und addiere X
    uint8_t value = readMemory(addr, 1);  // Lese den Wert
    value = LSR(value);                   // Führe logisches Shift-Right durch
    writeMemory(addr, value);             // Schreibe neuen Wert zurück
    cpu.PC++;
}

void OPCODE_5A(void) { cpu.PC++; }
