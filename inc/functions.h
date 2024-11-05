// Header-Datei: cpuFunctions.h

#ifndef CPU_FUNCTIONS_H
#define CPU_FUNCTIONS_H

#include <stdint.h>
extern uint8_t readMemory(uint16_t addr, uint16_t trace);
extern void writeMemory(uint16_t addr, uint8_t value);
extern uint8_t addrZeropage(void);
extern uint8_t addrZeropageX(void);
extern uint8_t addrZeropageY(void);
extern uint16_t addrImmediate(void);
extern uint16_t addrAbsolute(void);
extern uint16_t addrAbsoluteX(void);
extern uint16_t addrAbsoluteY(void);
extern uint16_t addrIndirect(void);
extern uint16_t addrIndirectX(void);
extern uint16_t addrIndirect_Y(void);

extern void setFlag(uint8_t flag, int value);
extern int getFlag(uint8_t flag);
extern void setFlagsqqq(uint8_t result);
extern void initStack(void);
extern void pushStack8(uint8_t value);
extern uint8_t popStack8(void);
extern void pushStack16(uint16_t value);
extern uint16_t popStack16(void);

extern int triggerIrq();
extern int triggerNIM();

// Prototypen für die Operationen auf dem Akkumulator
extern void ROR_A(void);           // Rotate Right auf den Akkumulator
extern void ADC_A(uint8_t value);  // Add with Carry auf den Akkumulator
extern void SBC_A(uint8_t value);  // Subtract with Carry auf den Akkumulator
extern void AND_A(uint8_t value);  // Logical AND auf den Akkumulator
extern void ORA_A(uint8_t value);  // Logical OR auf den Akkumulator
extern void EOR_A(uint8_t value);  // Logical Exclusive OR auf den Akkumulator
extern void BIT_A(uint8_t value);

// Prototypen für Akkumulator- und Speicheroperationen
extern uint8_t ASL(uint8_t value);
extern uint8_t LSR(uint8_t value);
extern void ROL_A(void);
extern void ROR_A(void);
extern uint8_t ROL(uint8_t value);
extern uint8_t ROR(uint8_t value);
extern uint8_t INC(uint8_t value);
extern uint8_t DEC(uint8_t value);
extern void BIT_A(uint8_t value);

#endif  // CPU_FUNCTIONS_H
