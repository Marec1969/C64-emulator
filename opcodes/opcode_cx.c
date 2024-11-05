#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_C0(void) {
    // CPY Immediate
    uint16_t addr = addrImmediate();  // Verwende addrImmediate für Immediate-Adressen
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.Y == value);
    setFlag(FLAG_NEGATIVE, cpu.Y < value);
    setFlag(FLAG_CARRY, cpu.Y >= value);
    cpu.PC++;
}

void OPCODE_C1(void) {
    // CMP (Indirect,X)
    uint16_t addr = addrIndirectX();  // Verwende addrIndirectX für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == value);
    setFlag(FLAG_NEGATIVE, cpu.A < value);
    setFlag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_C4(void) {
    // CPY Zero Page
    uint16_t addr = addrZeropage();  // Verwende addrZeropage für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.Y == value);
    setFlag(FLAG_NEGATIVE, cpu.Y < value);
    setFlag(FLAG_CARRY, cpu.Y >= value);
    cpu.PC++;
}

void OPCODE_C5(void) {
    // CMP Zero Page
    uint16_t addr = addrZeropage();  // Verwende addrZeropage für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == value);
    setFlag(FLAG_NEGATIVE, cpu.A < value);
    setFlag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_C6(void) {
    // DEC Zero Page
    uint16_t addr = addrZeropage();  // Verwende addrZeropage für die Adressierung
    uint8_t value = readMemory(addr, 1);
    value--;
    writeMemory(addr, value);
    setFlag(FLAG_ZERO, value == 0);
    setFlag(FLAG_NEGATIVE, value & 0x80);
    cpu.PC++;
}

void OPCODE_C8(void) {
    // INY (Increment Y Register)
    cpu.Y++;
    setFlag(FLAG_ZERO, cpu.Y == 0);
    setFlag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_C9(void) {
    // CMP Immediate
    uint16_t addr = addrImmediate();  // Verwende addrImmediate für Immediate-Adressen
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == value);
    setFlag(FLAG_NEGATIVE, cpu.A < value);
    setFlag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_CA(void) {
    // DEX (Decrement X Register)
    cpu.X--;
    setFlag(FLAG_ZERO, cpu.X == 0);
    setFlag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}

void OPCODE_CC(void) {
    // CPY Absolute
    uint16_t addr = addrAbsolute();  // Verwende addrAbsolute für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.Y == value);
    setFlag(FLAG_NEGATIVE, cpu.Y < value);
    setFlag(FLAG_CARRY, cpu.Y >= value);
    cpu.PC++;
}

void OPCODE_CD(void) {
    // CMP Absolute
    uint16_t addr = addrAbsolute();  // Verwende addrAbsolute für die Adressierung
    uint8_t value = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == value);
    setFlag(FLAG_NEGATIVE, cpu.A < value);
    setFlag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_CE(void) {
    // DEC Absolute
    uint16_t addr = addrAbsolute();  // Verwende addrAbsolute für die Adressierung
    uint8_t value = readMemory(addr, 1);
    value--;
    writeMemory(addr, value);
    setFlag(FLAG_ZERO, value == 0);
    setFlag(FLAG_NEGATIVE, value & 0x80);
    cpu.PC++;
}
