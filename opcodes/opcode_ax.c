
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_A0(void) {
    // LDY Immediate
    uint16_t addr = addrImmediate();
    cpu.Y = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.Y == 0);
    setFlag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_A1(void) {
    // LDA (Indirect,cpu.X)
    uint16_t addr = addrIndirectX();
    cpu.A = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == 0);
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_A2(void) {
    // LDX Immediate
    uint16_t addr = addrImmediate();
    cpu.X = readMemory(addr, 1);
    // printf("addr / value   %04x   %02x\r\n",addr,cpu.X);
    setFlag(FLAG_ZERO, cpu.X == 0);
    setFlag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}

void OPCODE_A4(void) {
    // LDY Zero Page
    uint16_t addr = addrZeropage();
    cpu.Y = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.Y == 0);
    setFlag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_A5(void) {
    // LDA Zero Page
    uint16_t addr = addrZeropage();
    cpu.A = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == 0);
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_A6(void) {
    // LDX Zero Page
    uint16_t addr = addrZeropage();
    cpu.X = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.X == 0);
    setFlag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}

void OPCODE_A8(void) {
    // TAY (Transfer Accumulator to Y)
    cpu.Y = cpu.A;
    setFlag(FLAG_ZERO, cpu.Y == 0);
    setFlag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_A9(void) {
    // LDA Immediate
    uint16_t addr = addrImmediate();
    cpu.A = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == 0);
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_AA(void) {
    // TAX (Transfer Accumulator to X)
    cpu.X = cpu.A;
    setFlag(FLAG_ZERO, cpu.X == 0);
    setFlag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}

void OPCODE_AC(void) {
    // LDY Absolute
    uint16_t addr = addrAbsolute();
    cpu.Y = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.Y == 0);
    setFlag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_AD(void) {
    // LDA Absolute
    uint16_t addr = addrAbsolute();
    cpu.A = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.A == 0);
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_AE(void) {
    // LDX Absolute
    uint16_t addr = addrAbsolute();
    cpu.X = readMemory(addr, 1);
    setFlag(FLAG_ZERO, cpu.X == 0);
    setFlag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}
