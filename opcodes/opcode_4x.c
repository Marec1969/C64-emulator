#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"
#include "trace.h"


void OPCODE_40(void) {
    // RTI (Return from Interrupt) - Rückkehr aus einem Interrupt
    updateTraceStopIRQ();   
    cpu.SR = (popStack8() | 0x20) & ~0x10; // set unused to 1 and break to 0;    // Prozessorstatus (Statusregister) wiederherstellen
    cpu.PC = popStack16();   // Programmzähler (PC) auf den Wert vom Stack setzen
}

void OPCODE_41(void) {
    // EOR (Indirect,X) - Exklusives ODER mit indirekter X-Adressierung
    uint16_t addr = addrIndirectX();                // Berechne die Adresse über Indirekte X-Adressierung
    EOR_A(readMemory(addr,1));                       // Führe XOR mit Akku und dem Speicherwert durch
    cpu.PC++;
}

void OPCODE_45(void) {
    // EOR Zero Page - Exklusives ODER auf der Zero Page
    uint16_t addr = addrZeropage();                  // Hole Adresse aus der Zero Page
    EOR_A(readMemory(addr,1));                        // Führe XOR mit Akku und Speicherwert aus
    cpu.PC++;
}

void OPCODE_46(void) {
    // LSR Zero Page - Logische Schiebung nach rechts auf der Zero Page
    uint16_t addr = addrZeropage();                  // Hole Adresse aus der Zero Page
    uint8_t value = readMemory(addr,1);               // Lese den Wert aus dem Speicher
    uint8_t newValue = LSR(value);                   // Führe die LSR-Operation durch
    writeMemory(addr, newValue);                    // Speichere den neuen Wert
    cpu.PC++;
}

void OPCODE_48(void) {
    // PHA (Push Accumulator) - Akku auf den Stack schieben
    pushStack8(cpu.A);  // Push den Akkumulator auf den Stack
    cpu.PC++;
}

void OPCODE_49(void) {
    // EOR  Immediate
    uint16_t addr = addrImmediate();                  // Hole Adresse aus der Zero Page
    EOR_A(readMemory(addr,1));                        // Führe XOR mit Akku und Speicherwert aus
    cpu.PC++;
}


void OPCODE_4A(void) {
    // LSR Accumulator - Logische Schiebung nach rechts im Akku
    cpu.A = LSR(cpu.A);    // Führe die LSR-Operation auf dem Akku durch
    cpu.PC++;
}

void OPCODE_4C(void) {
    // JMP Absolute - Unbedingter Sprung zur absoluten Adresse
    uint16_t addr = addrAbsolute();                   // Hole die absolute Adresse
    cpu.PC = addr;                                   // Setze den Programmzähler auf diese Adresse
}

void OPCODE_4D(void) {
    // EOR Absolute - Exklusives ODER mit absoluter Adressierung
    uint16_t addr = addrAbsolute();                   // Hole die absolute Adresse
    EOR_A(readMemory(addr,1));                         // Führe XOR mit Akku und Speicherwert aus
    cpu.PC++;
}

void OPCODE_4E(void) {
    // LSR Absolute - Logische Schiebung nach rechts auf der absoluten Adresse
    uint16_t addr = addrAbsolute();                   // Hole die absolute Adresse
    uint8_t value = readMemory(addr,1);               // Lese den Wert aus dem Speicher
    uint8_t newValue = LSR(value);                   // Führe die LSR-Operation durch
    writeMemory(addr, newValue);                    // Speichere den neuen Wert
    cpu.PC++;
}
