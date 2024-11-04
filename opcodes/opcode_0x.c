
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"
#include "cpurunner.h"
#include "trace.h"


void OPCODE_00(void) { 
uint16_t irqVector;
    // BRK (Beenden des Programms)
    // printf("BRK (Program beendet) pc %04x\n",cpu.PC);
    // setException(EXCEPTION_OPCODE);
    updateTraceStartIRQ();       
    cpu.PC++;
    cpu.PC++;
    cpu.SR |= 0x30;                 // merkwürdig eeigenschaft 
    pushStack16(cpu.PC);          // Speichere aktuellen PC
    pushStack8(cpu.SR);           // Speichere Statusregister
    cpu.SR |= FLAG_INTERRUPT;     // Setze Interrupt Disable Flag
    cpu.SR |= FLAG_BREAK;         // Setze Interrupt Disable Flag
    irqVector = readMemory(0xFFFE,1) | (readMemory(0xFFFF,1) << 8);
    cpu.PC = irqVector;           // Springe zur IRQ-Routine
}

void OPCODE_01(void) {
    // ORA (Indirect,cpu.X)
    uint16_t addr = addrIndirectX();
    ORA_A(readMemory(addr,1));
    cpu.PC++;
}


void OPCODE_05(void) {
    // ORA Zero Page
    uint16_t addr = addrZeropage();
    ORA_A(readMemory(addr,1));
    cpu.PC++;
}

void OPCODE_06(void) {
    // ASL Zero Page
    uint16_t addr = addrZeropage();
    uint8_t value = readMemory(addr,1);
    value = ASL(value);
    writeMemory(addr,value);
    cpu.PC++;
}

void OPCODE_08(void) {
    // PHP (Push Processor Status)
    pushStack8(cpu.SR | 0x30); // set unused and break to 1 
    cpu.PC++;
}

void OPCODE_09(void) {
    // ORA Immediate
    uint16_t addr = addrImmediate();
    ORA_A(readMemory(addr,1));
    cpu.PC++;
}

void OPCODE_0A(void) {
    // ASL Accumulator
    cpu.A = ASL(cpu.A);
    cpu.PC++;
}


void OPCODE_0D(void) {
    // ORA Absolute
    uint16_t addr = addrAbsolute();
    ORA_A(readMemory(addr,1));
    cpu.PC++;
}

void OPCODE_0E(void) {
    // ASL Absolute
    uint16_t addr = addrAbsolute();
    uint8_t value = readMemory(addr,1);

    value = ASL(value);
    writeMemory(addr,value);
    cpu.PC++;
}


