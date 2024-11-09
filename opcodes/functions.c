#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cia.h"
#include "cpu6510.h"
#include "cpurunner.h"
#include "trace.h"
#include "vic.h"
#include "sid.h"

extern const unsigned char prom[];
extern const unsigned char characters[];

// Hilfsfunktionen zum Setzen und Löschen von Flags im Statusregister

void setFlag(uint8_t flag, int value) {
    if (value) {
        cpu.SR |= flag;
    } else {
        cpu.SR &= ~flag;
    }
}

int getFlag(uint8_t flag) { return (cpu.SR & flag) ? 1 : 0; }

uint8_t readMemory(uint16_t addr, uint16_t trace) {
    uint8_t val = 0;        // Defaultwert für val
    uint8_t seg = MEM_RAM;  // Standardsegment, falls kein anderer Fall eintritt

    if ((memory[1] & 0x03) == 0) {  // bits 0,1 not set -> everything is RAM
        seg = MEM_RAM;
        val = memory[addr];
    } else {
        if ((addr >= 0xD000) && (addr < 0xE000)) {
            if ((memory[1] & 0x04) == 0x04) {
                if ((addr >= 0xD000) && (addr <= 0xD030)) {
                    val = readVic(addr);
                    seg = MEM_IO;
                } else if ((addr >= CIA1ADDR) && (addr <= CIA1END)) {
                    val = readCia1(addr);
                    seg = MEM_IO;
                } else if ((addr >= CIA2ADDR) && (addr <= CIA2END)) {
                    val = readCia2(addr);
                    seg = MEM_IO;
                } else if ((addr >= COLOR_ADDR) && (addr <= COLOR_ADDR_END)) {
                    val = colormap[addr - COLOR_ADDR];
                    seg = MEM_COLOR;
                } else if (addr >= 0xD400 && addr <= 0xD7FF) {                    
                    val = sidRead(addr);
                    seg = MEM_SID;
                } else {
                    val = rom[addr];
                    seg = MEM_ROM;
                }
            } else {
                val = characters[addr - CHAR_ROM_ADDR];
                seg = MEM_CHAR;
            }
        } else if ((addr >= 0xA000) && (addr < 0xC000)) {
            if ((memory[1] & 0x03) == 0x03) {
                val = rom[addr];
                seg = MEM_ROM;
            } else {
                val = memory[addr];
                seg = MEM_RAM;
            }
        } else if (addr >= 0xE000) {
            if ((memory[1] & 0x02) == 0x02) {
                val = rom[addr];
                seg = MEM_ROM;
            } else {
                val = memory[addr];
                seg = MEM_RAM;
            }
        } else {
            val = memory[addr];
            seg = MEM_RAM;
        }
    }
    // Einmaliger Aufruf der updateTraceRDMem-Funktion am Ende
    if (trace) {
        updateTraceRDMem(addr, val, seg);
    }
    return val;
}

void writeMemory(uint16_t addr, uint8_t value) {
    uint8_t seg = MEM_RAM;  // Standardsegment setzen

    /*
        if (addr<2) {
           printf("%04X write %02X   mem %02x\n",cpu.PC,addr,value );
        }
    */

    if ((memory[1] & 0x03) == 0) {  // bits 0,1 not set -> everything is RAM
        seg = MEM_RAM;
    } else {
        // Überprüfen, ob die Adresse im Bereich D000 - E000 liegt
        // only IO can be written ; if not IO it wil lbe writte to RAM
        if (addr >= 0xD000 && addr < 0xE000) {
            // Überprüfen, ob die RAM-Nutzung aktiviert ist
            if (memory[1] & 0x04) {  // Wenn Bit 2 gesetzt ist
                // I/O-Bereich überprüfen
                if (addr <= 0xD030) {
                    seg = MEM_IO;
                    writeVic(addr, value);
                } else if (addr >= CIA1ADDR && addr <= CIA1END) {
                    seg = MEM_IO;
                    writeCia1(addr, value);
                } else if (addr >= CIA2ADDR && addr <= CIA2END) {
                    seg = MEM_IO;
                    writeCia2(addr, value);
                } else if (addr >= COLOR_ADDR && addr <= COLOR_ADDR_END) {
                    seg = MEM_COLOR;
                    colormap[addr - COLOR_ADDR] = value;
                } else if (addr >= 0xD400 && addr <= 0xD7FF) {
                    // Sound Interface Chip (SID) - ignorieren, nicht implementiert
                    sidWrite(addr,value);
                    seg = MEM_SID;
                } else {
                    // Fehlerprotokollierung für nicht verwendete Adressen
                    printf("write to nothing! cpuport[1,2]=%02x:%02x addr=%04x val=%02x\n", memory[0], memory[1], addr,
                           value);
                    setException(EXCEPTION_WRITE_MEM);
                }
            }
        }
    }
    // Standardfall: Speicher im RAM aktualisieren
    if (seg == MEM_RAM) {
        memory[addr] = value;
    }

    // Einmaliger Aufruf am Ende zur Protokollierung
    updateTraceWRMem(addr, value, seg);
}

uint16_t addrImmediate(void) {
    // z.B OPCODE %A9 ->LDA #$nn
    return ++cpu.PC;
}

uint8_t addrZeropage(void) {
    // z.B OPCODE %A5 -> LDA $ll
    return readMemory(++cpu.PC, 0);
}

uint8_t addrZeropageX(void) {
    // z.B OPCODE %B5 -> LDA $ll,X
    return (readMemory(++cpu.PC, 0) + cpu.X) & 0xFF;
}

uint8_t addrZeropageY(void) {
    // z.B OPCODE %B6 -> LDX $ll,Y
    return (readMemory(++cpu.PC, 0) + cpu.Y) & 0xFF;
}

uint16_t addrAbsolute(void) {
    // z.B OPCODE %AD -> LDA $hhll
    uint8_t addrlo = readMemory(++cpu.PC, 0);
    uint8_t addrhi = readMemory(++cpu.PC, 0);
    return (addrhi << 8) | addrlo;
}

uint16_t addrAbsoluteX(void) {
    // z.B OPCODE %BD -> LDA $hhll,X
    uint8_t addrlo = readMemory(++cpu.PC, 0);
    uint8_t addrhi = readMemory(++cpu.PC, 0);
    uint16_t baseAddress = (addrhi << 8) | addrlo;
    uint16_t address = baseAddress + cpu.X;

    // Page Crossing prüfen (falls nötig, kann hier ein zusätzlicher Taktzyklus berücksichtigt werden)
    if ((baseAddress & 0xFF00) != (address & 0xFF00)) {
        clkCount++;
        // Seitenwechsel: hier könnte eine Zyklusverzögerung simuliert werden
    }

    return address;
}

uint16_t addrAbsoluteY(void) {
    // z.B OPCODE %B9 -> LDA $hhll,Y
    uint8_t addrlo = readMemory(++cpu.PC, 0);
    uint8_t addrhi = readMemory(++cpu.PC, 0);
    uint16_t baseAddress = (addrhi << 8) | addrlo;
    uint16_t address = baseAddress + cpu.Y;

    // Page Crossing prüfen
    if ((baseAddress & 0xFF00) != (address & 0xFF00)) {
        clkCount++;
        // Seitenwechsel: hier könnte eine Zyklusverzögerung simuliert werden
    }

    return address;
}

uint16_t addrIndirect(void) {
    // z.B OPCODE %6C -> JMP ($hhll)
    uint8_t addrlo = readMemory(++cpu.PC, 0);
    uint8_t addrhi = readMemory(++cpu.PC, 0);
    uint16_t indirectAddress = (addrhi << 8) | addrlo;

    uint8_t low = readMemory(indirectAddress, 1);
    uint8_t high = readMemory((indirectAddress & 0xFF) == 0xFF ? (indirectAddress & 0xFF00) : (indirectAddress + 1), 1);
    return (high << 8) | low;
}

uint16_t addrIndirectX(void) {
    // z.B OPCODE %A1 -> LDA ($ll, X)
    uint8_t baseAddress = readMemory(++cpu.PC, 0);
    uint8_t low = readMemory((baseAddress + cpu.X) & 0xFF, 1);
    //    uint8_t high = readMemory((baseAddress + 1) & 0xFF,1); // toDo: ich bin mir hier nicht sicher
    uint8_t high = readMemory((baseAddress + cpu.X + 1) & 0xFF, 1);
    return (high << 8) | low;
}

uint16_t addrIndirect_Y(void) {
    // z.B OPCODE %B1 -> LDA ($ll), Y
    uint8_t baseAddress = readMemory(++cpu.PC, 0);
    uint8_t low = readMemory(baseAddress, 1);
    uint8_t high = readMemory((baseAddress + 1) & 0xFF, 0);
    uint16_t targetAddr = ((high << 8) | low) + cpu.Y;

    // Page Crossing prüfen
    if (((high << 8) | low) & 0xFF00 != targetAddr & 0xFF00) {
        clkCount++;
        // Seitenwechsel: hier könnte eine Zyklusverzögerung simuliert werden
    }

    return targetAddr;
}

void initStack() {
    cpu.SP = 0xFF;  // Setzt den Stack Pointer auf den obersten Punkt
}

void pushStack16(uint16_t value) {
    // printf("push16 %04x\r\n", value);
    writeMemory(STACK_BASE + cpu.SP--, (value >> 8) & 0xFF);  // Push das höhere Byte zuerst
    writeMemory(STACK_BASE + cpu.SP--, value & 0xFF);         // Push das niedrigere Byte danach
}

uint16_t popStack16(void) {
    uint16_t value;
    uint8_t low = readMemory(STACK_BASE + ++cpu.SP, 1);   // Lese das niederwertige Byte
    uint8_t high = readMemory(STACK_BASE + ++cpu.SP, 1);  // Lese das höherwertige Byte
    value = (high << 8) | low;
    // printf("pop16 %04x\r\n", value);
    return value;  // Kombiniere High- und Low-Byte zu einer 16-Bit-Adresse
}

void pushStack8(uint8_t value) {
    // Schreibe den Wert in den Speicher an die Adresse, die durch den Stack Pointer angegeben wird
    writeMemory(STACK_BASE + cpu.SP, value);
    cpu.SP--;  // Verringere den Stack Pointer
}

uint8_t popStack8() {
    cpu.SP++;                                   // Erhöhe den Stack Pointer
    return readMemory(STACK_BASE + cpu.SP, 1);  // Lese den Wert aus dem Speicher
}

int triggerIrq() {
    uint16_t irqVector;

    if (!(cpu.SR & FLAG_INTERRUPT)) {  // IRQ nur auslösen, wenn I-Flag nicht gesetzt ist
        cpu.SR |= 0x20;                // set unused bit to 1
        pushStack16(cpu.PC);           // Speichere aktuellen PC
        pushStack8(cpu.SR);            // Speichere Statusregister
        cpu.SR |= FLAG_INTERRUPT;      // Setze Interrupt Disable Flag
        irqVector = readMemory(0xFFFE, 1) | (readMemory(0xFFFF, 1) << 8);
        cpu.PC = irqVector;  // Springe zur IRQ-Routine
        return 1;
    }
    return 0;
}

int triggerNIM() {
    pushStack16(cpu.PC);  // Speichere aktuellen PC
    pushStack8(cpu.SR);   // Speichere Statusregister
    // cpu.SR |= FLAG_INTERRUPT;      // Setze Interrupt Disable Flag

    uint16_t irqVector = readMemory(0xFFFA, 1) | (readMemory(0xFFFB, 1) << 8);
    cpu.PC = irqVector;  // Springe zur IRQ-Routine
    return 1;
}

// ROR (Rotate Right) auf den Akkumulator
void ROR_A(void) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 1 : 0;  // Hole das aktuelle Carry-Flag

    // Setze das neue Carry-Flag basierend auf dem niedrigsten Bit des Akkumulators
    setFlag(FLAG_CARRY, cpu.A & 1);

    // Führe die ROR-Operation auf den Akkumulator durch (rotate right)
    cpu.A = (cpu.A >> 1) | (carry << 7);

    // Setze die entsprechenden Flags
    setFlag(FLAG_ZERO, cpu.A == 0);        // Setze das Zero-Flag, wenn der Akkumulator 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);  // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// Funktion zur Durchführung von Linksrotation auf dem Akkumulator
void ROL_A(void) {
    uint8_t carry = getFlag(FLAG_CARRY);     // Hole Carry-Flag
    uint16_t result = (cpu.A << 1) | carry;  // Linksrotation mit Carry
    cpu.A = result & 0xFF;                   // Ergebnis im Akkumulator speichern
    setFlag(FLAG_CARRY, result > 0xFF);      // Carry setzen
    setFlag(FLAG_ZERO, cpu.A == 0);          // Setze das Zero-Flag, wenn der Akkumulator 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);    // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// ADC (Add with Carry) für den Akkumulator mit Unterscheidung zwischen Binär- und Dezimalmodus
void ADC_A(uint8_t value) {
    if (cpu.SR & FLAG_DECIMAL) {  // Dezimalmodus (BCD)
        // Hole den oberen und unteren Nibble von Akkumulator und Wert
        uint8_t lowNibble_a = cpu.A & 0x0F;
        uint8_t highNibble_a = cpu.A >> 4;
        uint8_t lowNibble_value = value & 0x0F;
        uint8_t highNibble_value = value >> 4;

        // Addiere die unteren Nibbles mit Carry
        uint8_t lowResult = lowNibble_a + lowNibble_value + (cpu.SR & FLAG_CARRY ? 1 : 0);
        if (lowResult > 9) {
            lowResult -= 10;  // Korrektur bei BCD
            highNibble_a++;   // Übertrag zu den höheren Nibbles
        }

        // Addiere die oberen Nibbles
        uint8_t highResult = highNibble_a + highNibble_value;
        if (highResult > 9) {
            highResult -= 10;        // Korrektur bei BCD
            setFlag(FLAG_CARRY, 1);  // Setze Carry, falls Übertrag auftritt
        } else {
            setFlag(FLAG_CARRY, 0);
        }

        // Setze das Ergebnis zusammen
        cpu.A = (highResult << 4) | (lowResult & 0x0F);

        // Setze die Flags
        setFlag(FLAG_ZERO, cpu.A == 0);
        setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
        // Overflow-Flag wird im Dezimalmodus nicht gesetzt

    } else {  // Binärmodus
        uint16_t result = cpu.A + value + (cpu.SR & FLAG_CARRY ? 1 : 0);

        // Setze die entsprechenden Flags
        setFlag(FLAG_CARRY, result > 0xFF);        // Carry-Flag, wenn das Ergebnis größer als 255 ist
        setFlag(FLAG_ZERO, (result & 0xFF) == 0);  // Zero-Flag, wenn das Ergebnis null ist
        setFlag(FLAG_NEGATIVE, result & 0x80);     // Negative-Flag basierend auf dem höchsten Bit des Ergebnisses
        setFlag(FLAG_OVERFLOW, (~(cpu.A ^ value) & (cpu.A ^ result) & 0x80));  // Overflow-Flag

        cpu.A = result & 0xFF;  // Speichere das Ergebnis im Akkumulator
    }
}

// SBC (Subtract with Carry) für den Akkumulator mit Unterscheidung zwischen Binär- und Dezimalmodus
void SBC_A(uint8_t value) {
    if (cpu.SR & FLAG_DECIMAL) {  // Dezimalmodus (BCD)
        // Hole den oberen und unteren Nibble von Akkumulator und Wert
        uint8_t lowNibble_a = cpu.A & 0x0F;
        uint8_t highNibble_a = cpu.A >> 4;
        uint8_t lowNibble_value = value & 0x0F;
        uint8_t highNibble_value = value >> 4;

        // Subtrahiere die unteren Nibbles mit Borrow
        uint8_t lowResult = lowNibble_a - lowNibble_value - (cpu.SR & FLAG_CARRY ? 0 : 1);
        if (lowResult & 0x10) {  // Wenn das Ergebnis negativ ist (Unterlauf)
            lowResult += 10;     // Korrigiere für BCD
            highNibble_a--;      // Borrow von den höheren Nibbles
        }

        // Subtrahiere die oberen Nibbles
        uint8_t highResult = highNibble_a - highNibble_value;
        if (highResult & 0x10) {     // Wenn das Ergebnis negativ ist (Unterlauf)
            highResult += 10;        // Korrektur bei BCD
            setFlag(FLAG_CARRY, 0);  // Setze Carry nicht
        } else {
            setFlag(FLAG_CARRY, 1);  // Setze Carry
        }

        // Setze das Ergebnis zusammen
        cpu.A = (highResult << 4) | (lowResult & 0x0F);

        // Setze die Flags
        setFlag(FLAG_ZERO, cpu.A == 0);
        setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
        // Overflow-Flag wird im Dezimalmodus nicht gesetzt

    } else {  // Binärmodus
        uint16_t result = cpu.A - value - (cpu.SR & FLAG_CARRY ? 0 : 1);

        // Setze die entsprechenden Flags
        setFlag(FLAG_CARRY,
                cpu.A >= (value + (cpu.SR & FLAG_CARRY ? 0 : 1)));  // Carry-Flag, wenn kein Unterlauf auftritt
        setFlag(FLAG_ZERO, (result & 0xFF) == 0);                   // Zero-Flag, wenn das Ergebnis null ist
        setFlag(FLAG_NEGATIVE, result & 0x80);                      // Negative-Flag basierend auf dem höchsten Bit
        setFlag(FLAG_OVERFLOW, ((cpu.A ^ result) & (cpu.A ^ value) & 0x80));  // Overflow-Flag

        cpu.A = result & 0xFF;  // Speichere das Ergebnis im Akkumulator
    }
}

// AND (Logical AND) für den Akkumulator
void AND_A(uint8_t value) {
    cpu.A = cpu.A & value;  // Führe das logische UND mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    setFlag(FLAG_ZERO, cpu.A == 0);        // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);  // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// ORA (Logical OR) für den Akkumulator
void ORA_A(uint8_t value) {
    cpu.A = cpu.A | value;  // Führe das logische ODER mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    setFlag(FLAG_ZERO, cpu.A == 0);        // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);  // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// EOR (Logical Exclusive OR) für den Akkumulator
void EOR_A(uint8_t value) {
    cpu.A = cpu.A ^ value;  // Führe das logische XOR mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    setFlag(FLAG_ZERO, cpu.A == 0);        // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80);  // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// ASL (Arithmetic Shift Left) - Verschiebt einen Wert um 1 Bit nach links
uint8_t ASL(uint8_t value) {
    setFlag(FLAG_CARRY, value & 0x80);     // Setze Carry, wenn das höchste Bit gesetzt ist
    value = (value << 1) & 0xff;           // Schiebe um 1 Bit nach links
    setFlag(FLAG_ZERO, value == 0);        // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);  // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// LSR (Logical Shift Right) - Verschiebt einen Wert um 1 Bit nach rechts
uint8_t LSR(uint8_t value) {
    setFlag(FLAG_CARRY, value & 0x01);     // Setze Carry basierend auf dem niedrigsten Bit
    value >>= 1;                           // Schiebe um 1 Bit nach rechts
    setFlag(FLAG_ZERO, value == 0);        // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);  // Negative-Flag, obwohl logisch
    return value;
}

// ROL (Rotate Left) - Rotiert einen Wert nach links (mit Carry)
uint8_t ROL(uint8_t value) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 1 : 0;  // Hole das aktuelle Carry-Flag
    setFlag(FLAG_CARRY, value & 0x80);              // Setze Carry, wenn das höchste Bit gesetzt ist
    value = ((value << 1) | carry) & 0xff;          // Rotieren nach links und Carry ins niedrigste Bit setzen
    setFlag(FLAG_ZERO, value == 0);                 // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);           // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// ROR (Rotate Right) - Rotiert einen Wert nach rechts (mit Carry)
uint8_t ROR(uint8_t value) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 0x80 : 0;  // Hole das Carry-Flag und verschiebe es ins höchste Bit
    setFlag(FLAG_CARRY, value & 0x01);                 // Setze Carry, wenn das niedrigste Bit gesetzt ist
    value = (value >> 1) | carry;                      // Rotieren nach rechts und Carry ins höchste Bit setzen
    setFlag(FLAG_ZERO, value == 0);                    // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);              // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// INC (Increment) - Erhöht einen Wert um 1
uint8_t INC(uint8_t value) {
    value += 1;                            // Inkrementiere den Wert
    setFlag(FLAG_ZERO, value == 0);        // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);  // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// DEC (Decrement) - Verringert einen Wert um 1
uint8_t DEC(uint8_t value) {
    value -= 1;                            // Dekrementiere den Wert
    setFlag(FLAG_ZERO, value == 0);        // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);  // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// Funktion zur Durchführung von BIT-Operation auf dem Akkumulator
void BIT_A(uint8_t value) {
    setFlag(FLAG_ZERO, (cpu.A & value) == 0);  // Setze Zero-Flag
    setFlag(FLAG_NEGATIVE, value & 0x80);      // Setze Negative-Flag (bit 7 von value)
    setFlag(FLAG_OVERFLOW, value & 0x40);      // Setze Overflow-Flag (bit 6 von value)
}
