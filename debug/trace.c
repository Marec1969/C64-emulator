
#include "trace.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "vic.h"

#define DOINCLUDE_CPUDATA
#include "cpurunner.h"
#undef DOINCLUDE_CPUDATA

#define TRACE_OPCODE 0x01
#define TRACE_MEM 0x02
#define TRACE_VIC 0x04
#define TRACE_IRQ 0x08
#define TRACE_CPU_REG 0x10
#define TRACE_IO 0x20

trace_t trace[MAXTRACE];
uint16_t traceLevel = TRACE_IRQ | TRACE_OPCODE | TRACE_CPU_REG | TRACE_MEM;  // ; TRACE_IO |

uint32_t traceRunner = 0;
int32_t traceInIRQ = 0;

extern uint32_t raster;

static void updateTraceOpcode0(uint8_t opcode) {
    trace[traceRunner].action = ACTION_OPCODE_0;
    trace[traceRunner].opcode = opcode;

    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;

    trace[traceRunner].a = cpu.A;
    trace[traceRunner].x = cpu.X;
    trace[traceRunner].y = cpu.Y;
    trace[traceRunner].sr = cpu.SR;
    trace[traceRunner].pc = cpu.PC;
    trace[traceRunner].sp = cpu.SP;

    traceRunner = (traceRunner + 1) % MAXTRACE;
}

static void updateTraceOpcode1(uint8_t opcode, uint8_t data1) {
    trace[traceRunner].action = ACTION_OPCODE_1;
    trace[traceRunner].opcode = opcode;
    trace[traceRunner].data1 = data1;

    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;

    trace[traceRunner].a = cpu.A;
    trace[traceRunner].x = cpu.X;
    trace[traceRunner].y = cpu.Y;
    trace[traceRunner].sr = cpu.SR;
    trace[traceRunner].pc = cpu.PC;
    trace[traceRunner].sp = cpu.SP;

    traceRunner = (traceRunner + 1) % MAXTRACE;
}

static void updateTraceOpcode2(uint8_t opcode, uint8_t data1, uint8_t data2) {
    trace[traceRunner].action = ACTION_OPCODE_2;
    trace[traceRunner].opcode = opcode;
    trace[traceRunner].data1 = data1;
    trace[traceRunner].data2 = data2;

    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;

    trace[traceRunner].a = cpu.A;
    trace[traceRunner].x = cpu.X;
    trace[traceRunner].y = cpu.Y;
    trace[traceRunner].sr = cpu.SR;
    trace[traceRunner].pc = cpu.PC;
    trace[traceRunner].sp = cpu.SP;

    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void updateTraceOpcode(uint8_t opcode, uint8_t data1, uint8_t data2) {
    if ((traceLevel & TRACE_OPCODE) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    switch (opcodeLengths[opcode]) {
        case 0:
            printf("illegal opcode \r\n%04x\t%02X\t%s\t%02X\r\n", cpu.PC, opcode, opcodes[opcode * 2], cpu.X);
            setException(EXCEPTION_ILEGAL);
            break;
        case 1:
            updateTraceOpcode0(opcode);
            break;
        case 2:
            updateTraceOpcode1(opcode, data1);
            break;
        case 3:
            updateTraceOpcode2(opcode, data1, data2);
            break;
    }
}

void updateTraceRDMem(uint16_t addr, uint8_t value, uint8_t seg) {
    if (seg == MEM_IO)
        updateTraceRDIO(addr, value);

    if ((traceLevel & TRACE_MEM) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    trace[traceRunner].action = ACTION_RD_MEM;
    trace[traceRunner].addr = addr;
    trace[traceRunner].value = value;
    trace[traceRunner].seg = seg;
    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;
    trace[traceRunner].cpuPort1 = memory[0];
    trace[traceRunner].cpuPort2 = memory[1];
    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void updateTraceWRMem(uint16_t addr, uint8_t value, uint8_t seg) {
    if (seg == MEM_IO)
        updateTraceWRIO(addr, value);

    if ((traceLevel & TRACE_MEM) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    trace[traceRunner].action = ACTION_WR_MEM;
    trace[traceRunner].addr = addr;
    trace[traceRunner].value = value;
    trace[traceRunner].seg = seg;
    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;
    trace[traceRunner].cpuPort1 = memory[0];
    trace[traceRunner].cpuPort2 = memory[1];
    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void updateTraceWRVIC(uint16_t addr, uint8_t value) {
    if ((traceLevel & TRACE_VIC) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    trace[traceRunner].action = ACTION_WR_VIC;
    trace[traceRunner].addr = addr;
    trace[traceRunner].value = value;
    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;
    trace[traceRunner].cpuPort1 = memory[0];
    trace[traceRunner].cpuPort2 = memory[1];
    trace[traceRunner].vicCtrl1 = vicRegisters.control1;
    trace[traceRunner].vicCtrl2 = vicRegisters.control2;
    trace[traceRunner].vicMemCtrl = vicRegisters.memoryControl;

    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void updateTraceRDVIC(uint16_t addr, uint8_t value) {
    if ((traceLevel & TRACE_VIC) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    trace[traceRunner].action = ACTION_RD_VIC;
    trace[traceRunner].addr = addr;
    trace[traceRunner].value = value;
    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;
    trace[traceRunner].cpuPort1 = memory[0];
    trace[traceRunner].cpuPort2 = memory[1];
    trace[traceRunner].vicCtrl1 = vicRegisters.control1;
    trace[traceRunner].vicCtrl2 = vicRegisters.control2;
    trace[traceRunner].vicMemCtrl = vicRegisters.memoryControl;

    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void updateTraceWRIO(uint16_t addr, uint8_t value) {
    if ((traceLevel & TRACE_IO) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    trace[traceRunner].action = ACTION_WR_IO;
    trace[traceRunner].addr = addr;
    trace[traceRunner].value = value;
    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;
    trace[traceRunner].cpuPort1 = memory[0];
    trace[traceRunner].cpuPort2 = memory[1];

    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void updateTraceRDIO(uint16_t addr, uint8_t value) {
    if ((traceLevel & TRACE_IO) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    trace[traceRunner].action = ACTION_RD_IO;
    trace[traceRunner].addr = addr;
    trace[traceRunner].value = value;
    trace[traceRunner].raster = raster;
    trace[traceRunner].irq = doIRQ;
    trace[traceRunner].cpuPort1 = memory[0];
    trace[traceRunner].cpuPort2 = memory[1];

    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void updateTraceStartIRQ(void) {
    if ((traceLevel & TRACE_IRQ) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    traceInIRQ++;  // if IRQs are nested (???)
    trace[traceRunner].action = ACTION_START_IRQ;
    trace[traceRunner].inIrq = traceInIRQ;
    trace[traceRunner].irq = doIRQ;
    if (traceInIRQ >= 5) {
        setException(EXCEPTION_TRACE);
    }

    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void updateTraceStopIRQ(void) {
    if ((traceLevel & TRACE_IRQ) == 0)
        return;

    trace[traceRunner].cycle = clkCount;

    traceInIRQ--;  // if IRQs are nested (???)
    trace[traceRunner].action = ACTION_END_IRQ;
    trace[traceRunner].inIrq = traceInIRQ;
    trace[traceRunner].irq = doIRQ;
    if (traceInIRQ <= 0) {
        traceInIRQ = 0;
        // setException(EXCEPTION_TRACE);
    }
    traceRunner = (traceRunner + 1) % MAXTRACE;
}

void addTabs(FILE* filePointer, int cnt) {
    fprintf(filePointer, "\t\t\t");
    for (int16_t x = 0; x < cnt; x++) {
        fprintf(filePointer, "\t\t");
    }
}

void printCPURegs(FILE* filePointer, int32_t pos) {
    if (traceLevel & TRACE_CPU_REG) {
        addTabs(filePointer, traceInIRQ + 3);
        fprintf(filePointer, "a=%02X\tx=%02X\ty=%02X\tsr=%02X\tSP=%02X\n", trace[pos].a, trace[pos].x, trace[pos].y,
                trace[pos].sr, trace[pos].sp);
    }
}

void writeTrace(void) {
    FILE* filePointer = fopen("trace.txt", "w");

    // Überprüfen, ob die Datei erfolgreich geöffnet wurde
    if (filePointer == NULL) {
        printf("Fehler beim Erstellen der Datei.\n");
        return;
    }

    printf("Start write Tracedata\n");

    for (int32_t run = traceRunner; run < (traceRunner + MAXTRACE); run++) {
        if ((run % 100000) == 0) {
            printf(".\n");
        }
        int32_t i = run % MAXTRACE;

        fprintf(filePointer, "T=%3.3f\t", (double)trace[i].cycle * 1e-6);

        switch (trace[i].action) {
            case ACTION_OPCODE_0:
                printCPURegs(filePointer, i);
                addTabs(filePointer, traceInIRQ);
                fprintf(filePointer, "%04X\t%02X\t%s\n", trace[i].pc, trace[i].opcode, opcodes[trace[i].opcode].m1);
                break;
            case ACTION_OPCODE_1:
                printCPURegs(filePointer, i);
                addTabs(filePointer, traceInIRQ);
                fprintf(filePointer, "%04X\t%02X\t%s$%02X%s\n", trace[i].pc, trace[i].opcode,
                        opcodes[trace[i].opcode].m1, trace[i].data1, opcodes[trace[i].opcode].m2);
                break;
            case ACTION_OPCODE_2:
                printCPURegs(filePointer, i);
                addTabs(filePointer, traceInIRQ);
                fprintf(filePointer, "%04X\t%02X\t%s$%02X%02X%s\n", trace[i].pc, trace[i].opcode,
                        opcodes[trace[i].opcode].m1, trace[i].data2, trace[i].data1, opcodes[trace[i].opcode].m2);
                break;
            case ACTION_RD_MEM:
                addTabs(filePointer, traceInIRQ + 3);
                fprintf(filePointer, "rd mem %04X\t%02x\t%02x:%02x  from %02x\n", trace[i].addr, trace[i].value,
                        trace[i].cpuPort1, trace[i].cpuPort2, trace[i].seg);
                break;
            case ACTION_WR_MEM:
                addTabs(filePointer, traceInIRQ + 3);
                fprintf(filePointer, "wr mem %04X\t%02x\t%02x:%02x  to %02x\n", trace[i].addr, trace[i].value,
                        trace[i].cpuPort1, trace[i].cpuPort2, trace[i].seg);
                break;
            case ACTION_START_IRQ:
                traceInIRQ++;
                addTabs(filePointer, traceInIRQ);
                fprintf(filePointer, "Start IRQ %d  %02X\n", trace[i].inIrq, trace[i].irq);
                break;
            case ACTION_END_IRQ:
                addTabs(filePointer, traceInIRQ);
                fprintf(filePointer, "End IRQ %d %02X\n", trace[i].inIrq, trace[i].irq);
                traceInIRQ--;
                break;
            case ACTION_RD_IO:
                addTabs(filePointer, traceInIRQ + 3);
                fprintf(filePointer, "IO\trd mem %04X\t%02x\t%02x:%02x\n", trace[i].addr, trace[i].value,
                        trace[i].cpuPort1, trace[i].cpuPort2, trace[i].seg);
                break;
            case ACTION_WR_IO:
                addTabs(filePointer, traceInIRQ + 3);
                fprintf(filePointer, "IO\t\t\t\twr mem %04X\t%02x\t%02x:%02x\n", trace[i].addr, trace[i].value,
                        trace[i].cpuPort1, trace[i].cpuPort2, trace[i].seg);
                break;
        }
    }
    fclose(filePointer);
    printf("Write Tracedata to Disk finished\n");
}