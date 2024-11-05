#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vic.h>

#include "cpu6510.h"

extern uint8_t characters[];

void saveMemory(void) {
    // Datei öffnen (oder erstellen) im "w" Modus (write)
    FILE* filePointer = fopen("memory.txt", "w");

    // Überprüfen, ob die Datei erfolgreich geöffnet wurde
    if (filePointer == NULL) {
        printf("Fehler beim Erstellen der Datei.\n");
        return;
    }

    fprintf(filePointer, "A %02x\t", cpu.A);
    fprintf(filePointer, "X %02x\t", cpu.X);
    fprintf(filePointer, "Y %02x\t", cpu.Y);

    fprintf(filePointer, "SR %02x\t", cpu.SR);
    fprintf(filePointer, "PC %04x\t", cpu.PC);

    int cnt = 0x00;
    while (cnt < 0x10000) {
        fprintf(filePointer, "\n%04x  ", cnt);
        for (int j = 0; j < 32; j++) {
            fprintf(filePointer, "%02x ", memory[cnt]);
            cnt++;
        }
        fprintf(filePointer, "\t\t\t");
        cnt -= 32;
        for (int j = 0; j < 32; j++) {
            if (memory[cnt] > 31) {
                fprintf(filePointer, "%c", memory[cnt]);
            } else {
                fprintf(filePointer, " ");
            }
            cnt++;
        }
    }
    fprintf(filePointer, "\n\n\nColormap\n");
    cnt = 0;
    while (cnt < 1000) {
        fprintf(filePointer, "\n%04x  ", 0xd800 + cnt);
        for (int j = 0; j < 32; j++) {
            fprintf(filePointer, "%02x ", colormap[cnt]);
            cnt++;
        }
    }

    fprintf(filePointer, "\n");

    cnt = 0;
    while (cnt < 4096) {
        fprintf(filePointer, "\n%04x  ", 0xd800 + cnt);
        for (int j = 0; j < 32; j++) {
            fprintf(filePointer, "%02x ", characters[cnt]);
            cnt++;
        }
    }

    fprintf(filePointer, "\n");

    // Datei schließen
    fclose(filePointer);

    printf("Datei erfolgreich erstellt und geschrieben.\n");

    return;
}
