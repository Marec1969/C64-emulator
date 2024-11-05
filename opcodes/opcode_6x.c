#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

// RTS (Return from Subroutine)
void OPCODE_60(void) {
    cpu.PC = popStack16() + 1;  // Rücksprungadresse vom Stack holen und 1 addieren
}

void OPCODE_61(void) {
    // ADC (Indirect,X)
    uint16_t addr = addrIndirectX();      // Berechne die indirekte Adresse
    uint8_t value = readMemory(addr, 1);  // Lese den Wert von der berechneten Adresse
    ADC_A(value);                         // Führe die Addition durch
    cpu.PC++;
}

// ADC Zero Page
void OPCODE_65(void) {
    uint16_t addr = addrZeropage();       // Lese Zero-Page-Adresse
    uint8_t value = readMemory(addr, 1);  // Lese den Wert von der berechneten Adresse
    ADC_A(value);                         // Führe die Addition durch
    cpu.PC++;
}

// ROR Zero Page
void OPCODE_66(void) {
    uint16_t addr = addrZeropage();       // Lese die Adresse aus dem Zero Page
    uint8_t value = readMemory(addr, 1);  // Lese den Wert von der Adresse
    value = ROR(value);                   // Führe ROR (Rotate Right) durch
    writeMemory(addr, value);             // Schreibe den neuen Wert zurück
    cpu.PC++;
}

// PLA (Pull Accumulator)
void OPCODE_68(void) {
    cpu.A = popStack8();  // Wert vom Stack in den Akkumulator holen
    setFlag(FLAG_ZERO, cpu.A == 0);
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;  // toDo: flages sind nich beeiflusst ???
}

// ADC Immediate
void OPCODE_69(void) {
    uint16_t addr = addrImmediate();
    uint8_t value = readMemory(addr, 1);  // Lese den Wert von der berechneten Adresse
    ADC_A(value);                         // Führe die Addition durch
    cpu.PC++;
}

// ROR Accumulator
void OPCODE_6A(void) {
    cpu.A = ROR(cpu.A);  // ROR (Rotate Right) durch den Akkumulator
    cpu.PC++;
}

// JMP (Indirect)
void OPCODE_6C(void) {
    uint16_t addr = addrIndirect();  // Lese die indirekte Adresse
    cpu.PC = addr;                   // Setze die Program Counter auf die neue Adresse
}

// ADC Absolute
void OPCODE_6D(void) {
    uint16_t addr = addrAbsolute();       // Lese die absolute Adresse
    uint8_t value = readMemory(addr, 1);  // Lese den Wert von der berechneten Adresse
    ADC_A(value);                         // Führe die Addition durch
    cpu.PC++;
}

// ROR Absolute
void OPCODE_6E(void) {
    uint16_t addr = addrAbsolute();       // Lese die absolute Adresse
    uint8_t value = readMemory(addr, 1);  // Lese den Wert von der Adresse
    value = ROR(value);                   // Führe ROR (Rotate Right) durch
    writeMemory(addr, value);             // Schreibe den neuen Wert zurück
    cpu.PC++;
}
