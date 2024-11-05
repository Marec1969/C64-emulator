#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_30(void) {
    // BMI (Branch if Minus) - Verzweigung bei negativem Flag
    cpu.PC++;
    if (cpu.SR & FLAG_NEGATIVE) {
        cpu.PC += (int8_t)readMemory(cpu.PC, 1);  // Zweig ausführen, wenn das Negative-Flag gesetzt ist
        cpu.PC++;
        clkCount++;
    } else {
        cpu.PC++;
    }
}

void OPCODE_31(void) {
    // AND (Indirect),Y - Logisches UND mit indirekter Y-Adressierung
    uint16_t addr = addrIndirect_Y();     // Verwende addrIndirect_Y für die Adresse
    uint8_t value = readMemory(addr, 1);  // Lese den Wert aus dem Speicher
    AND_A(value);                         // Führe das logische UND auf den Akkumulator aus
    cpu.PC++;
}

void OPCODE_35(void) {
    // AND Zero Page,X - Logisches UND mit Zero Page und X-Register
    uint16_t addr = addrZeropageX();      // Verwende addrZeropageX
    uint8_t value = readMemory(addr, 1);  // Lese den Wert aus dem Speicher
    AND_A(value);                         // Führe das logische UND auf den Akkumulator aus
    cpu.PC++;
}

void OPCODE_36(void) {
    // ROL Zero Page,X - Linksrotation mit Zero Page und X-Register
    uint16_t addr = addrZeropageX();  // Verwende addrZeropageX
    uint8_t value = readMemory(addr, 1);
    uint8_t result = ROL(value);  // Führe die Linksrotation durch und speichere das Ergebnis
    writeMemory(addr, result);    // Schreibe den neuen Wert zurück in den Speicher
    cpu.PC++;
}

void OPCODE_38(void) {
    // SEC - Set Carry Flag (Setze das Carry-Flag)
    setFlag(FLAG_CARRY, 1);  // Setze das Carry-Flag
    cpu.PC++;
}

void OPCODE_39(void) {
    // AND Absolute,Y - Logisches UND mit absoluter X-Adressierung
    uint16_t addr = addrAbsoluteY();      // Verwende addrAbsoluteY
    uint8_t value = readMemory(addr, 1);  // Lese den Wert aus dem Speicher
    AND_A(value);                         // Führe das logische UND auf den Akkumulator aus
    cpu.PC++;
}

void OPCODE_3D(void) {
    // AND Absolute,X - Logisches UND mit absoluter X-Adressierung
    uint16_t addr = addrAbsoluteX();      // Verwende addrAbsoluteX
    uint8_t value = readMemory(addr, 1);  // Lese den Wert aus dem Speicher
    AND_A(value);                         // Führe das logische UND auf den Akkumulator aus
    cpu.PC++;
}

void OPCODE_3E(void) {
    // ROL Absolute,X - Linksrotation mit absoluter X-Adressierung
    uint16_t addr = addrAbsoluteX();  // Verwende addrAbsoluteX
    uint8_t value = readMemory(addr, 1);
    uint8_t result = ROL(value);  // Führe die Linksrotation durch und speichere das Ergebnis
    writeMemory(addr, result);    // Schreibe den neuen Wert zurück in den Speicher
    cpu.PC++;
}
