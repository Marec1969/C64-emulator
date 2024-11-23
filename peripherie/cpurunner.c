#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEASURE_PERFORMANZE

#ifdef MEASURE_PERFORMANZE
#include <windows.h>
static LARGE_INTEGER frequency;
static LARGE_INTEGER start, end;
#endif

#include "characters.h"
#include "cpu6510.h"

#define DOINCLUDE_CPUDATA
#include "cpurunner.h"
#undef DOINCLUDE_CPUDATA

#include "cia.h"
#include "main.h"
#include "saveMemory.h"
#include "trace.h"
#include "vic.h"
#include "floppy1570.h"

extern uint64_t gesTsc;

int irqCnt = 0;
uint32_t clkCount = 0;
static uint16_t exception = 0;

char startDir[MAX_PATH];

uint8_t memory[MEMORY_SIZE];  // Speicher
uint8_t rom[MEMORY_SIZE];     // Speicher
unsigned char characters[CHARACTER_LEN];
;

CPU cpu;

int16_t doIRQ = 0;
int16_t doNIM = 1;

void SetWorkingDirectory(const char* path) { SetCurrentDirectoryA(path); }

// CPU-Reset
void resetCpu() {
    memset(&cpu, 0x00, sizeof(cpu));
    memory[0] = 0x2f;
    memory[1] = 0xe7;
    initStack();
}

// Funktion zum Laden der Binärdatei in den Speicher
unsigned char* read_binary_file(const char* filename, uint8_t* buffer, size_t filesize) {
    FILE* file = fopen(filename, "rb");  // Datei im Binärmodus öffnen
    if (!file) {
        perror("Datei konnte nicht geöffnet werden");
        return NULL;
    }

    fseek(file, 0, SEEK_SET);

    // Datei in den Puffer einlesen
    size_t fileRead = fread(buffer, 1, filesize, file);
    if (fileRead != filesize) {
        printf("Fehler beim Einlesen der Datei %zu : %zu\n", fileRead, filesize);
        fclose(file);
        return NULL;
    }

    fclose(file);
    printf("Read succsess %s %zu\n", filename, filesize);
    return buffer;
}

void loadRom() {
    read_binary_file("../rom/basic.rom", (uint8_t*)&rom[BASIC_ROM_ADDR], BASIC_ROM_LEN);
    read_binary_file("../rom/kernel.rom", (uint8_t*)&rom[CORE_ROM_ADDR], CORE_ROM_LEN);
    read_binary_file("../rom/character.rom", (uint8_t*)characters, sizeof(characters));

    cpu.PC = (readMemory(0XFFFC, 0) | (readMemory(0XFFFD, 0) << 8));  // Reset Vector
}

void cpuRunnerInit() {
    printf("Start\n");
    GetCurrentDirectoryA(MAX_PATH, startDir);

    // CPU und Speicher initialisieren
    resetCpu();
    loadRom();
    cpu.PC = 0x080d;
    cpu.PC = (readMemory(0XFFFC, 0) | (readMemory(0XFFFD, 0) << 8));  // Reset Vector
    printf("Start Prog at %04X  %02x\r\n", cpu.PC, cpu.SR);
}

void setException(int16_t from) {
    exception = from;
    printf("Set exception from %d\n", from);
    mainStop(0);
}

void exitCPURunner(void) {
    extern void saveSong(void);
#ifdef MEASURE_PERFORMANZE
    // uint64_t endTsc = rdtsc();
    QueryPerformanceCounter(&end);  // Endzeitpunkt
    // printf("TSC-Differenz: %3.3f Zyklen\n", (double)(endTsc - startTsc)*0.4e-9);
    double elapsed = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Zeit: %3.2f Sekunden\n", elapsed);
#endif

    printf("TSC-Differenz: %3.3f Zyklen\n", (double)(gesTsc) * 0.4e-9 * 0.45);

    printf("Ende time %3.2fS   irq Cnt=%d\r\n", (double)clkCount * 1e-6, irqCnt);
    printf("Ende at %d \n", clkCount);

    {
        extern volatile int vicUpdateCnt;
        extern volatile int update;
        printf("vic update = %d \t doIRQ=%04x\t clkCount %d\n", vicUpdateCnt, doIRQ, clkCount);
    }

    // Arbeitsverzeichnis zurücksetzen
    SetWorkingDirectory(startDir);

    /*
    saveSong();
    saveScreen();
    writeVic_registers_to_file();
    saveMemory();
    writeCia1toTxtFile();
    writeCia2toTxtFile();
    */
    writeTrace();
    saveMemory();
    writeCia1toTxtFile();
    writeCia2toTxtFile();
}

void cpuRunnerDo(void) {
    uint8_t OPCODE;
    static int oldptr = -1;
    static int16_t oldNIM = 1;
    static int16_t oldIRQ = 0;
    uint32_t oldClkCount = 0;
    uint32_t clkDiff = 0;
    int run = 0;

#ifdef MEASURE_PERFORMANZE
    QueryPerformanceFrequency(&frequency);  // Frequenz des Hochleistungszählers
    QueryPerformanceCounter(&start);        // Startzeitpunkt
#endif

    while (1) {
        if (!mainRunning()) {
            exitCPURunner();
            return;
        }

        oldptr = cpu.PC;
        OPCODE = readMemory(cpu.PC, 0);

        if (doIRQ) {
            if (triggerIrq()) {
                irqCnt++;
                updateTraceStartIRQ();
                OPCODE = readMemory(cpu.PC, 0);
                clkCount += 7;
            }
        }

        if ((doNIM == 0) && (oldNIM)) {
            triggerNIM();
            updateTraceStartIRQ();
            OPCODE = readMemory(cpu.PC, 0);
            clkCount += 7;
        }
        oldNIM = doNIM;

        updateTraceOpcode(OPCODE, readMemory(oldptr + 1, 0), readMemory(oldptr + 2, 0));
        jumptable[OPCODE]();  // Direkter Sprung zu der OPCODE-Handler-Funktion

        clkCount += opcodeCycles[OPCODE];
        clkDiff = clkCount - oldClkCount;
        oldClkCount = clkCount;

        updateCia1(clkDiff);
        updateCia2(clkDiff);
        updateVic(clkCount);
        device1570Update();
    }
}
