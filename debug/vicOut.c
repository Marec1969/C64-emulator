
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "vic.h"

extern uint32_t raster;

void writeVic_registers_to_file(void) {
    VIC_II_Registers* regs = &vicRegisters;
    FILE* file = fopen("civRegiser.txt", "w");
    if (file == NULL) {
        printf("Fehler beim Öffnen der Datei!\n");
        return;
    }
#if 0
    fprintf(file, "Address\tName\t\t\tValue\n");
    fprintf(file, "------------------------------------------\n");

    fprintf(file, "$D000\tSprite 0 X-Position\t\t0x%02X\n", regs->sprite0_x_pos);
    fprintf(file, "$D001\tSprite 0 Y-Position\t\t0x%02X\n", regs->sprite0_y_pos);
    fprintf(file, "$D002\tSprite 1 X-Position\t\t0x%02X\n", regs->sprite1_x_pos);
    fprintf(file, "$D003\tSprite 1 Y-Position\t\t0x%02X\n", regs->sprite1_y_pos);
    fprintf(file, "$D004\tSprite 2 X-Position\t\t0x%02X\n", regs->sprite2_x_pos);
    fprintf(file, "$D005\tSprite 2 Y-Position\t\t0x%02X\n", regs->sprite2_y_pos);
    fprintf(file, "$D006\tSprite 3 X-Position\t\t0x%02X\n", regs->sprite3_x_pos);
    fprintf(file, "$D007\tSprite 3 Y-Position\t\t0x%02X\n", regs->sprite3_y_pos);
    fprintf(file, "$D008\tSprite 4 X-Position\t\t0x%02X\n", regs->sprite4_x_pos);
    fprintf(file, "$D009\tSprite 4 Y-Position\t\t0x%02X\n", regs->sprite4_y_pos);
    fprintf(file, "$D00A\tSprite 5 X-Position\t\t0x%02X\n", regs->sprite5_x_pos);
    fprintf(file, "$D00B\tSprite 5 Y-Position\t\t0x%02X\n", regs->sprite5_y_pos);
    fprintf(file, "$D00C\tSprite 6 X-Position\t\t0x%02X\n", regs->sprite6_x_pos);
    fprintf(file, "$D00D\tSprite 6 Y-Position\t\t0x%02X\n", regs->sprite6_y_pos);
    fprintf(file, "$D00E\tSprite 7 X-Position\t\t0x%02X\n", regs->sprite7_x_pos);
    fprintf(file, "$D00F\tSprite 7 Y-Position\t\t0x%02X\n", regs->sprite7_y_pos);
    fprintf(file, "$D010\tSprite X MSB\t\t\t0x%02X\n", regs->spriteX_msb);
    fprintf(file, "$D011\tControl Register 1\t\t0x%02X\n", regs->control1);
    fprintf(file, "$D012\tRaster Line\t\t\t0x%02X\n", regs->rasterLine);
    fprintf(file, "$D013\tLightpen X Coordinate\t\t0x%02X\n", regs->lightpenX);
    fprintf(file, "$D014\tLightpen Y Coordinate\t\t0x%02X\n", regs->lightpenY);
    fprintf(file, "$D015\tSprite Enable\t\t\t0x%02X\n", regs->spriteEnable);
    fprintf(file, "$D016\tControl Register 2\t\t0x%02X\n", regs->control2);
    fprintf(file, "$D017\tSprite Double Height\t\t0x%02X\n", regs->spriteDouble_height);
    fprintf(file, "$D018\tMemory Control\t\t\t0x%02X\n", regs->memoryControl);
    fprintf(file, "$D019\tIRQ Status\t\t\t0x%02X\n", regs->irqStatus);
    fprintf(file, "$D01A\tIRQ Mask\t\t\t0x%02X\n", regs->irqMask);
    fprintf(file, "$D01B\tSprite Priority\t\t\t0x%02X\n", regs->spritePriority);
    fprintf(file, "$D01C\tSprite Multicolor\t\t0x%02X\n", regs->spriteMulticolor);
    fprintf(file, "$D01D\tSprite Double Width\t\t0x%02X\n", regs->spriteDouble_width);
    fprintf(file, "$D01E\tSprite Collision\t\t0x%02X\n", regs->spriteCollision);
    fprintf(file, "$D01F\tSprite Background Collision\t0x%02X\n", regs->spriteBackground_collision);
    fprintf(file, "$D020\tBorder Color\t\t\t0x%02X\n", regs->borderColor);
    fprintf(file, "$D021\tBackground Color\t\t0x%02X\n", regs->backgroundColor);
    fprintf(file, "$D022\tBackground Color 1\t\t0x%02X\n", regs->backgroundColor1);
    fprintf(file, "$D023\tBackground Color 2\t\t0x%02X\n", regs->backgroundColor2);
    fprintf(file, "$D024\tBackground Color 3\t\t0x%02X\n", regs->backgroundColor3);
    fprintf(file, "$D025\tSprite Multicolor 0\t\t0x%02X\n", regs->spriteMulticolor0);
    fprintf(file, "$D026\tSprite Multicolor 1\t\t0x%02X\n", regs->spriteMulticolor1);

    for (int i = 0; i < 8; i++) {
        fprintf(file, "$%04x\tSprite %d Color\t\t0x%02X\n", i+0xd027, i, regs->spriteColor[i]);
    }

    fprintf(file, "$D02F\tKeyboard Pins\t\t\t0x%02X\n", regs->keyboardPins);
    fprintf(file, "$D030\tMode\t\t\t\t0x%02X\n", regs->mode);
#else

    fprintf(file, "Address\tName\t\t\tValue\t\tBit Description\n");
    fprintf(file, "--------------------------------------------------------------------\n");

    // Sprite X/Y Positions (D000 - D00F)
    for (int i = 0; i < 8; i++) {
        fprintf(file, "%04X\tSprite %d X-Position\t%d\n", 0xd000 + i * 2, i, regs->spritePos[i][0]);
        fprintf(file, "%04X\tSprite %d Y-Position\t%d\n\n", 0xd001 + i * 2, i, regs->spritePos[i][1]);
    }

    // $D010: X-Position MSB for all Sprites
    fprintf(file, "$D010\tSprite X MSB\t\t0x%02X\t\tBit 0-7: MSB of X position for sprites 0-7\n", regs->spriteX_msb);

    // $D011: Control Register 1
    fprintf(file, "$D011\tControl Register 1\t0x%02X\t\t", regs->control1);
    fprintf(file, "Bit 7: 9th bit of raster\n\t\t\t\t\tBit 6: Extended Color Mode\n");
    fprintf(file, "\t\t\t\t\tBit 5: Bitmap Mode\n\t\t\t\t\tBit 4: Screen on/off\n");
    fprintf(file, "\t\t\t\t\tBit 3: 25 lines (1) or 24 lines (0)\n");
    fprintf(file, "\t\t\t\t\tBit 2-0: Raster offset from top\n");

    // $D012: Raster Line
    fprintf(file, "$D012\tRaster Line\t\t0x%02X\t\tReading: current line, Writing: line for IRQ\n", regs->rasterLine);

    // $D013: Lightpen X-Coordinate
    fprintf(file, "$D013\tLightpen X-Coordinate\t0x%02X\n", regs->lightpenX);

    // $D014: Lightpen Y-Coordinate
    fprintf(file, "$D014\tLightpen Y-Coordinate\t0x%02X\n", regs->lightpenY);

    // $D015: Sprite Enable Register
    fprintf(file, "$D015\tSprite Enable\t\t0x%02X\t\tBit 0-7: Enable sprites 0-7 (1=on)\n", regs->spriteEnable);

    // $D016: Control Register 2
    fprintf(file, "$D016\tControl Register 2\t0x%02X\t\t", regs->control2);
    fprintf(file, "Bit 7-5: unused\n\t\t\t\t\tBit 4: Multicolor Mode (Text & Graphics)\n");
    fprintf(file, "\t\t\t\t\tBit 3: 40 columns (1) or 38 columns (0)\n");
    fprintf(file, "\t\t\t\t\tBit 2-0: Pixel offset from left\n");

    // $D017: Sprite Double Height
    fprintf(file, "$D017\tSprite Double Height\t0x%02X\t\tBit 0-7: Double height sprites 0-7\n",
            regs->spriteDouble_height);

    // $D018: Memory Control
    fprintf(file, "$D018\tMemory Control\t\t0x%02X\t\t", regs->memoryControl);
    fprintf(file, "Bit 7-4: Screen memory location\n\t\t\t\t\tBit 3-1: Charset memory location\n");
    fprintf(file, "\t\t\t\t\tBit 3: Bitmap mode memory location\n");

    // $D019: IRQ Status
    fprintf(file, "$D019\tIRQ Status\t\t0x%02X\t\t", regs->irqStatus);
    fprintf(file, "Bit 7: IRQ occurred\n\t\t\t\t\tBit 3: Lightpen IRQ\n");
    fprintf(file, "\t\t\t\t\tBit 2: Sprite-Sprite collision IRQ\n\t\t\t\t\tBit 1: Sprite-background collision IRQ\n");
    fprintf(file, "\t\t\t\t\tBit 0: Raster IRQ\n");

    // $D01A: IRQ Mask
    fprintf(file, "$D01A\tIRQ Mask\t\t0x%02X\t\t", regs->irqMask);
    fprintf(file, "Bit 3: Lightpen IRQ enable\n\t\t\t\t\tBit 2: Sprite-Sprite collision IRQ enable\n");
    fprintf(file, "\t\t\t\t\tBit 1: Sprite-background collision IRQ enable\n\t\t\t\t\tBit 0: Raster IRQ enable\n");

    // $D01B: Sprite Priority
    fprintf(file, "$D01B\tSprite Priority\t\t0x%02X\t\tBit 0-7: Sprite priority (1=behind background)\n",
            regs->spriteBackground_priority);

    // $D01C: Sprite Multicolor Mode
    fprintf(file, "$D01C\tSprite Multicolor Mode\t0x%02X\t\tBit 0-7: Sprite multicolor mode (1=multicolor)\n",
            regs->spriteMulticolor);

    // $D01D: Sprite Double Width
    fprintf(file, "$D01D\tSprite Double Width\t0x%02X\t\tBit 0-7: Double width sprites (1=double width)\n",
            regs->spriteDouble_width);

    // $D01E: Sprite-Sprite Collision
    fprintf(file, "$D01E\tSprite-Sprite Collision\t0x%02X\t\tBit 0-7: Sprite-sprite collision (1=collision)\n",
            regs->spriteCollision);

    // $D01F: Sprite-Background Collision
    fprintf(file, "$D01F\tSprite-Background Collision\t0x%02X\t\tBit 0-7: Sprite-background collision (1=collision)\n",
            regs->spriteBackground_collision);

    // $D020: Border Color
    fprintf(file, "$D020\tBorder Color\t\t0x%02X\n", regs->borderColor);

    // $D021: Background Color
    fprintf(file, "$D021\tBackground Color\t0x%02X\n", regs->backgroundColor);

    // $D022: Background Color 1
    fprintf(file, "$D022\tBackground Color 1\t0x%02X\n", regs->backgroundColor1);

    // $D023: Background Color 2
    fprintf(file, "$D023\tBackground Color 2\t0x%02X\n", regs->backgroundColor2);

    // $D024: Background Color 3
    fprintf(file, "$D024\tBackground Color 3\t0x%02X\n", regs->backgroundColor3);

    // $D025: Sprite Multicolor 0
    fprintf(file, "$D025\tSprite Multicolor 0\t0x%02X\n", regs->spriteMulticolor0);

    // $D026: Sprite Multicolor 1
    fprintf(file, "$D026\tSprite Multicolor 1\t0x%02X\n", regs->spriteMulticolor1);

    // Sprite colors ($D027 - $D02E)
    for (int i = 0; i < 8; i++) {
        fprintf(file, "$D027 + %d\tSprite %d Color\t\t0x%02X\n", i, i, regs->spriteColor[i]);
    }

    // $D02F: Keyboard Pins
    fprintf(file, "$D02F\tKeyboard Pins\t\t0x%02X\t\tBit 2-0: Keyboard pin status\n", regs->keyboardPins);

    // $D030: Mode
    fprintf(file, "$D030\tMode\t\t\t0x%02X\t\tBit 1: Test mode, Bit 0: 1MHz (0) or 2MHz (1)\n", regs->mode);

    fprintf(file, "Raster IRQ (help) = %d\n", regs->rasterCMP);
    fprintf(file, "Raster = %d\n", raster);

#endif

    fclose(file);
    printf("Register wurden in die Datei geschrieben.\n");
}

int saveScreen() {
    extern uint8_t memory[];  // Speicher
    // Datei öffnen (oder erstellen) im "w" Modus (write)
    FILE* filePointer = fopen("screen.txt", "w");

    // Überprüfen, ob die Datei erfolgreich geöffnet wurde
    if (filePointer == NULL) {
        printf("Fehler beim Erstellen der Datei.\n");
        return 1;
    }

    int cnt = 0x0400;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 40; j++) {
            if (memory[cnt] > 31) {
                fprintf(filePointer, "%c", memory[cnt]);
            } else {
                fprintf(filePointer, "%c", memory[cnt] + 'A' - 1);
            }
            cnt++;
        }
        fprintf(filePointer, "\n");
    }

    fprintf(filePointer, "\n\n\n\n");

    cnt = 0x4400;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 40; j++) {
            if (memory[cnt] > 31) {
                fprintf(filePointer, "%c", memory[cnt]);
            } else {
                fprintf(filePointer, "%c", memory[cnt] + 'A' - 1);
            }
            cnt++;
        }
        fprintf(filePointer, "\n");
    }

    // Datei schließen
    fclose(filePointer);

    printf("Datei erfolgreich erstellt und geschrieben.\n");

    return 0;
}
