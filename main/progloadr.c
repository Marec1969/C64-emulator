

#include <windows.h>
#include <commdlg.h>
#include <stdint.h>
#include <stdio.h>

#include "cpu6510.h"
#include "progloader.h"

#define MAX_BUFFER 0x10000
uint8_t buffer[MAX_BUFFER];

static char filename[MAX_PATH] = "";  // Speicher für den Dateinamen

unsigned char* getFilename() {
    OPENFILENAME ofn;  // Struktur für den Dateidialog

    // Struktur mit NULL initialisieren
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = "Alle Dateien\0*.*\0Binärdateien (*.bin)\0*.BIN\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Dialog öffnen
    if (GetOpenFileName(&ofn) == TRUE) {
        printf("Ausgewählte Datei: %s\n", filename);
        return filename;
    } else {
        printf("Kein Dateiname ausgewählt.\n");
    }

    return 0;
}

// Funktion zum Laden der Binärdatei in den Speicher
unsigned char* load_binary_file(const char* filename, size_t* filesize) {
    FILE* file = fopen(filename, "rb");  // Datei im Binärmodus öffnen
    if (!file) {
        perror("Datei konnte nicht geöffnet werden");
        return NULL;
    }

    // Größe der Datei bestimmen
    fseek(file, 0, SEEK_END);
    *filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (*filesize >= MAX_BUFFER) {
        return 0;
    }

    // Datei in den Puffer einlesen
    size_t bytesRead = fread(buffer, 1, *filesize, file);
    if (bytesRead != *filesize) {
        perror("Fehler beim Einlesen der Datei");
        fclose(file);
        return NULL;
    }

    fclose(file);
    return buffer;
}

int stopIRQ = 0;
extern int16_t doIRQ;
static void sloadPrg(uint8_t* ptr, size_t size) {
    int i;
    uint8_t* dest;

    t64Head_t* t64Head = (t64Head_t*)ptr;

    if (strncmp(t64Head->signature, "C64", 3) != 0) {
        for (i = 0; i < size; i++) {
            memory[i + 0x400] = *ptr++;
        }
        printf("Load binary to 0x400  irq stat %02x\n", doIRQ);
        cpu.PC = 0x400;
        doIRQ |= 0x1000;
        memory[1] = 0;
        cpu.SR |= FLAG_INTERRUPT;  // Setze Interrupt Disable Flag
        return;
    }

    t64Cunk_t* t64Cunk = (t64Cunk_t*)(ptr + sizeof(t64Head_t));
#if 0
    printf("size of Head %d / cunk %d\n",sizeof(t64Head_t),sizeof(t64Cunk_t));
    printf("Load Prg %s ver %d\tStart %04X\tEnd %04X \t offset %04X\n",t64Cunk->name,t64Cunk->type,t64Cunk->startAddr,t64Cunk->endAddr,t64Cunk->offset);
#endif
    printf("Load Prg %s ver %d\tmaxItems %d\trealItems %d\n", t64Head->signature, t64Head->version, t64Head->maxItems, t64Head->realItems);

    ptr += t64Cunk->offset;
    size = t64Cunk->endAddr - t64Cunk->startAddr;
    dest = memory + t64Cunk->startAddr;

    for (i = 0; i < size; i++) {
        *dest++ = *ptr++;
    }
    printf("Load %s\nfrom %04X to %04X   size %04X\toffest %04X\n", t64Cunk->name, t64Cunk->startAddr, t64Cunk->endAddr,(uint32_t) size, t64Cunk->offset);
}

void loadPrg(int rawKey) {
    size_t size;
    uint8_t* scrPtr;
    uint8_t* name;

    switch (rawKey) {
        case 122:

            break;
        case 123:
            name = getFilename();
            if (name) {
                scrPtr = load_binary_file(name, &size);
                if (scrPtr) {
                    sloadPrg(scrPtr, size);
                } else {
                    printf("Error laod Game\n");
                }
            }
            break;

        default:
            break;
    }
}
