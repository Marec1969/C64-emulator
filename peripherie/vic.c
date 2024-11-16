
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
// #include <sched.h>

#include "characters.h"
#include "cia.h"
#include "cpu6510.h"
#include "cpurunner.h"
#include "vic.h"

#define INLUDE_KEYMAP
#include "cpuCnt.h"
#include "mainWin.h"

#define NUM_BITMAPS 256
#define GRID_WIDTH 40
#define GRID_HEIGHT 25
#define PIXEL_SIZE 1

#define CHAR_SIZE_X 8
#define CHAR_SIZE_Y 8

#define NUM_FGCOLORS 16
#define NUM_BGCOLORS 16

VIC_II_Registers vicRegisters;

uint8_t colormap[1024];
uint8_t windowsScreen[PAL_B_Y][PAL_B_X];

volatile int vicUpdateCnt = 0;
volatile int update = 0;

uint64_t gesTsc = 0;

static uint64_t endTsc = 0;
static uint64_t startTsc = 0;

int16_t rasterXpos;
uint32_t raster;


#include <intrin.h>  // Für _mm_pause()

void busyWaitMicroseconds(double microseconds) {
    LARGE_INTEGER frequency, start, current;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    double targetSeconds = microseconds / 1e6;
    double elapsed = 0.0;

    while (elapsed < targetSeconds) {
        _mm_pause();  // CPU-optimierte Pause
        QueryPerformanceCounter(&current);
        elapsed = (double)(current.QuadPart - start.QuadPart) / frequency.QuadPart;
    }
}

void printBits(unsigned char num) {
    // Schleife über die Bits von Bit 7 bis Bit 0 (insgesamt 8 Bits)
    for (int i = 7; i >= 0; i--) {
        // Prüfen, ob das i-te Bit gesetzt ist, und 1 oder 0 ausgeben
        printf("%d", (num >> i) & 1);
    }
}

void writeVic(uint16_t addr, uint8_t value) {
    uint8_t* ptr = (uint8_t*)&vicRegisters;

    ptr += (addr - 0xd000);
    if (addr == 0xD011) {
        vicRegisters.rasterCMP = vicRegisters.rasterCMP & 0x1ff;  // nur vorsichtshalber
        if (value & 0x80) {
            vicRegisters.rasterCMP |= 0x100;
        } else {
            vicRegisters.rasterCMP &= ~0x0100;
        }
        // printf("Set high %02X to %d\n",value,vicRegisters.rasterCMP);
    }

    if (addr == 0xD018) {
        vicRegisters.memoryControl = value;
        // printf("Set memoryControl %02X\n",value,vicRegisters.memoryControl);
    }

    if (addr == 0xD012) {
        vicRegisters.rasterCMP = (vicRegisters.rasterCMP & 0x100) | value;
        // printf("Set low %02X to %d\n",value,vicRegisters.rasterCMP);
    }

    if (addr == 0xD019) {
        vicRegisters.irqStatus &= ~value;
        doIRQ &= ~0x01;
        if ((vicRegisters.irqStatus & 0x07) == 0) {
            vicRegisters.irqStatus = 0;
        }
        return;
    }

    *ptr = value;
}

uint8_t readVic(uint16_t addr) {
    uint8_t* ptr = (uint8_t*)&vicRegisters;
    uint8_t val;

    ptr += (addr - 0xd000);
    val = *ptr;

    if (addr == 0xD01E) {
        // printf("VIC spriteCollision IQR %02x\n",vicRegisters.spriteCollision);
        val = vicRegisters.spriteCollision;
        vicRegisters.spriteCollision = 0;
    }
    if (addr == 0xD01F) {
        // printf("VIC backCollision IQR %02x\n",vicRegisters.spriteBackground_collision);
        val = vicRegisters.spriteBackground_collision;
        vicRegisters.spriteBackground_collision = 0;
    }

    return (val);
}

static inline void drawMulticolorText(uint8_t fgColor, uint8_t bgColor, uint8_t* charBitPtr, int bitRow, int col) {
    uint8_t mask = 0xC0;
    uint8_t charBits = *charBitPtr++;
    for (int x = 0; x < 8; x += 2) {
        uint8_t color;
        switch (mask & charBits) {
            case 0xC0:
                color = (fgColor & 0x07) | 0x10;
                break;
            case 0x80:
                color = vicRegisters.backgroundColor2 | 0x10;
                break;
            case 0x40:
                color = vicRegisters.backgroundColor1;
                break;
            default:
                color = bgColor;
        }
        charBits <<= 2;
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + (vicRegisters.control2 & 0x07)] = color;
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + 1 + (vicRegisters.control2 & 0x07)] = color;
    }
}

static inline void drawMonochromeText(uint8_t fgColor, uint8_t bgColor, uint8_t* charBitPtr, int bitRow, int col) {
    uint8_t mask = 0x80;
    uint8_t charBits = *charBitPtr++;
    for (int x = 0; x < 8; x++) {
        uint8_t color = (mask & charBits) ? (fgColor | 0x10) : bgColor;
        mask >>= 1;
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + (vicRegisters.control2 & 0x07)] = color;
    }
}

static inline void drawMonochromeGrafik(uint8_t fgColor, uint8_t bgColor, uint8_t* charBitPtr, int bitRow, int col) {
    uint8_t mask = 0x80;
    uint8_t charBits = *charBitPtr++;
    for (int x = 0; x < 8; x++) {
        uint8_t color = (mask & charBits) ? (fgColor | 0x10) : bgColor;
        mask >>= 1;
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + (vicRegisters.control2 & 0x07)] = color;
    }
}

static inline void drawMulticolorGrafik(uint8_t fgColorxx, uint8_t fgColor11, uint8_t bgColor, uint8_t* charBitPtr,
                                        int bitRow, int col) {
    uint8_t mask = 0xC0;
    uint8_t charBits = *charBitPtr++;
    for (int x = 0; x < 8; x += 2) {
        uint8_t color;
        switch (mask & charBits) {
            case 0xC0:
                color = (fgColor11 & 0x07) | 0x10;
                break;
            case 0x80:
                color = fgColorxx & 0x0f;
                break;
            case 0x40:
                color = ((fgColorxx >> 4) & 0x0f) | 0x10;
                break;
            default:
                color = bgColor;
        }
        charBits <<= 2;
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + (vicRegisters.control2 & 0x07)] = color;
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + 1 + (vicRegisters.control2 & 0x07)] = color;
    }
}

static inline uint8_t* getCharBasePtr(uint8_t bank, uint16_t addr, uint16_t bankOffset) {
    if ((bank == 0) || (bank == 2)) {
        if (addr == 0x1000) {
            return &characters[0];  //  &rom[CHAR_ROM_ADDR];
        }
        if (addr == 0x1800) {
            return &characters[0x800];  // &rom[CHAR_ROM_ADDR+0x800];
        }
    }

    return &memory[bankOffset + addr];
}

void drawCharLine(uint16_t raster) {
    static uint8_t charBuffer[GRID_WIDTH];
    static uint8_t colorBuffer[GRID_WIDTH];
    static uint16_t bitRow = BR_TOP;
    static uint16_t charYPos = 0;
    static uint16_t charYRow = 0;
    static uint8_t* charBasePtr = &memory[0];

    if ((raster < BR_TOP) || (raster >= BR_BOTTOM))
        return;

    uint8_t bank = ciaGetvidoebank();
    uint16_t addr = ((vicRegisters.memoryControl >> 1) & 0x7) * 0x0800;
    uint16_t bankOffset = bank * 0x4000;
    uint16_t row = (raster / 8) - (BR_TOP / 8);

    if ((vicRegisters.control1 & 0x07) == (raster & 0x07)) {  // This is an Bad-Line :-)
        // printf("Badline %d\n",raster);
        uint16_t screenAddr = bankOffset + ((vicRegisters.memoryControl >> 4) & 0xf) * SCREENSIZE_BYTE;
        for (int col = 0; col < GRID_WIDTH; col++) {
            charBuffer[col] = memory[screenAddr + row * GRID_WIDTH + col];
            colorBuffer[col] = colormap[row * GRID_WIDTH + col] & 0x0F;
        }
        charBasePtr = getCharBasePtr(bank, addr, bankOffset);
        if (row == 0) {
            charYRow = 0;
        } else {
            charYRow += GRID_WIDTH * 8;
        }
        bitRow = raster;
        charYPos = 0;
        clkCount += 25;  // BadLine ca 25 Taktzykeln ausgelassen
    } else {
        bitRow++;
        charYPos++;
    }

    if ((vicRegisters.control1 & (EXTENDEDBACKCOLOR | C64BITMAP)) ==
        (EXTENDEDBACKCOLOR | C64BITMAP)) {  // toDo: ist wohl falsch -hier muss nur die Zeile in Schwarz gemacht werden
        for (int col = 0; col < GRID_WIDTH; col++) {
            uint8_t character = 0;
            uint8_t fgColor = 0;
            uint8_t bgColor = 0;
            uint8_t* charBitPtr = charBasePtr + character * 8 + charYPos;

            drawMonochromeText(fgColor, bgColor, charBitPtr, bitRow, col);
        }
        return;
    }

    for (int col = 0; col < GRID_WIDTH; col++) {
        uint8_t character = charBuffer[col];
        uint8_t fgColor = colorBuffer[col];
        uint8_t bgColor = vicRegisters.backgroundColor;
        uint8_t* charBitPtr = charBasePtr + character * 8 + charYPos;

        if (vicRegisters.control1 & EXTENDEDBACKCOLOR) {
            switch (character & 0xc0) {
                case 0xC0:
                    vicRegisters.backgroundColor3;
                    break;
                case 0x80:
                    vicRegisters.backgroundColor2;
                    break;
                case 0x40:
                    vicRegisters.backgroundColor1;
                    break;
                case 0x00:
                    vicRegisters.backgroundColor;
                    break;
            }
            character = character & 0x3f;
        }
        if (vicRegisters.control1 & C64BITMAP) {
            uint16_t startAddr = bankOffset + ((vicRegisters.memoryControl >> 3) & 0x1) * 0x2000;
            uint8_t bitColor = character;
            charBitPtr = &memory[startAddr + charYRow + col * 8 + charYPos];
            if (vicRegisters.control2 & MULTICOLORTEXT) {
                drawMulticolorGrafik(bitColor, fgColor, bgColor, charBitPtr, bitRow, col);
            } else {
                drawMonochromeGrafik(fgColor, bgColor, charBitPtr, bitRow, col);
            }
        } else {
            if ((vicRegisters.control2 & MULTICOLORTEXT) && (fgColor & 0x08)) {
                drawMulticolorText(fgColor, bgColor, charBitPtr, bitRow, col);
            } else {
                drawMonochromeText(fgColor, bgColor, charBitPtr, bitRow, col);
            }
        }
    }
}

void drawBoarderLine(uint16_t raster) {
    uint8_t brCol = vicRegisters.borderColor;

    uint8_t addX = 8;
    uint8_t addY = 4;

    if (vicRegisters.control1 & 0x08) {  // 25 oder 24 Zeilen
        addY = 0;
    }
    if (vicRegisters.control2 & 0x08) {  // 40 oder 38 Zeilen
        addX = 0;
    }

    if (raster < BR_TOP + addY) {
        for (int x = 0; x < PAL_B_X; x++) {
            windowsScreen[raster][x] = brCol;
        }
        return;
    }

    if (raster >= BR_BOTTOM - addY) {
        for (int x = 0; x < PAL_B_X; x++) {
            windowsScreen[raster][x] = brCol;
        }
        return;
    }

    for (int x = BR_RIGHT - addX; x < PAL_B_X; x++) {
        windowsScreen[raster][x] = brCol;
    }

    for (int x = 0; x < BR_LEFT + addX; x++) {
        windowsScreen[raster][x] = brCol;
    }
}

static inline void checkCollision(uint8_t* collisionBuffer, int16_t rasterPosY, int16_t spriteX, uint8_t spriteNrMask,
                                  uint8_t from) {
    if (collisionBuffer[spriteX]) {
        if ((collisionBuffer[spriteX] & spriteNrMask) == 0) {  // kollisionen mit sich selbst verbieten
            vicRegisters.spriteCollision |= collisionBuffer[spriteX];
            vicRegisters.spriteCollision |= spriteNrMask;
        } else {
            printf("Sprite %02x at %d from %d\n", spriteNrMask, spriteX, from);  // darf gar nicht forkommen
        }
    } else {
        collisionBuffer[spriteX] = spriteNrMask;
    }

    uint8_t fgColor = windowsScreen[rasterPosY][spriteX];  // Farben mit 0x10 Bit sind Zeichen Fordergrundfarben
    if (vicRegisters.spriteBackground_priority & spriteNrMask) {
        if ((fgColor & 0x10) && (collisionBuffer[spriteX])) {
            vicRegisters.spriteBackground_collision |= spriteNrMask;
        }
    } else {
        if ((fgColor != vicRegisters.backgroundColor) && (collisionBuffer[spriteX])) {
            vicRegisters.spriteBackground_collision |= spriteNrMask;
        }
    }
}

static inline void drawPixel(int16_t rasterPosY, int16_t* spriteX, uint8_t color, uint8_t* collisionBuffer,
                             uint8_t spriteNrMask, int doubleWidth) {
    if (vicRegisters.spriteBackground_priority & spriteNrMask) {
        checkCollision(collisionBuffer, rasterPosY, *spriteX, spriteNrMask, doubleWidth);
        if (windowsScreen[rasterPosY][*spriteX] & 0x10) {  // Sprite nicht malen
            (*spriteX)++;
            ;
        } else {
            windowsScreen[rasterPosY][(*spriteX)++] = color;
        }

        if (doubleWidth) {
            checkCollision(collisionBuffer, rasterPosY, *spriteX, spriteNrMask, 2);
            if (windowsScreen[rasterPosY][*spriteX] & 0x10) {  // Sprite nicht malen
                (*spriteX)++;
            } else {
                windowsScreen[rasterPosY][(*spriteX)++] = color;
            }
        }
    } else {
        checkCollision(collisionBuffer, rasterPosY, *spriteX, spriteNrMask, 3);
        windowsScreen[rasterPosY][(*spriteX)++] = color;
        if (doubleWidth) {
            checkCollision(collisionBuffer, rasterPosY, *spriteX, spriteNrMask, 4);
            windowsScreen[rasterPosY][(*spriteX)++] = color;
        }
    }
}

void updateSpriteLine(int16_t rasterPosY) {
    if (rasterPosY >= PAL_B_Y || vicRegisters.spriteEnable == 0)
        return;  // Screen Limit und Sprite Enable Check

    uint8_t bank = ciaGetvidoebank();
    uint16_t bankOfset = bank * 0x4000;
    uint16_t screenAddr = bankOfset + ((vicRegisters.memoryControl >> 4) & 0xf) * SCREENSIZE_BYTE;
    uint8_t collisionBuffer[PAL_B_X] = {0};  // Kollisionspuffer initialisieren

    for (int spriteNr = 0; spriteNr < 8; spriteNr++) {
        uint8_t spriteNrMask = 1 << spriteNr;
        if (vicRegisters.spriteEnable & spriteNrMask) {
            int16_t spriteY = vicRegisters.spritePos[spriteNr][INDEX_Y] + SPRITE_OFFSET_Y;
            int16_t distance = rasterPosY - spriteY;
            if (distance < 0)
                continue;

            int doubleHeight = vicRegisters.spriteDouble_height & spriteNrMask;
            int16_t sizeY = doubleHeight ? 2 * SPRITE_Y_SIZE : SPRITE_Y_SIZE;
            if (distance >= sizeY)
                continue;

            int doubleWidth = vicRegisters.spriteDouble_width & spriteNrMask;
            int16_t sizeX = doubleWidth ? 2 * SPRITE_X_SIZE : SPRITE_X_SIZE;

            int16_t spriteX = vicRegisters.spritePos[spriteNr][INDEX_X] + SPRITE_OFFSET_X;
            if (vicRegisters.spriteX_msb & spriteNrMask)
                spriteX += 256;

            if (spriteX > 350)
                continue;

            int multicolor = vicRegisters.spriteMulticolor & spriteNrMask;
            uint16_t spritePtr = memory[screenAddr + SCREENSIZE_BYTE - MAX_SPRITES + spriteNr] * 64;
            spritePtr += (doubleHeight ? (distance >> 1) : distance) * SPRITE_X_BYTE;

            for (int sByte = 0; sByte < SPRITE_X_BYTE; sByte++) {
                uint8_t val = memory[bankOfset + spritePtr++];
                if (multicolor) {
                    for (int bitCnt = 0; bitCnt < 8; bitCnt += 2) {
                        uint8_t bitMask = (val >> (6 - bitCnt)) & 0x03;
                        if (bitMask) {
                            uint8_t color = (bitMask == 3)   ? vicRegisters.spriteMulticolor1
                                            : (bitMask == 2) ? vicRegisters.spriteColor[spriteNr]
                                                             : vicRegisters.spriteMulticolor0;
                            drawPixel(rasterPosY, &spriteX, color, collisionBuffer, spriteNrMask, doubleWidth);
                            drawPixel(rasterPosY, &spriteX, color, collisionBuffer, spriteNrMask, doubleWidth);
                        } else
                            spriteX += doubleWidth ? 4 : 2;
                    }
                } else {
                    for (int bitCnt = 0; bitCnt < 8; bitCnt++) {
                        if (val & (0x80 >> bitCnt)) {
                            uint8_t color = vicRegisters.spriteColor[spriteNr];
                            drawPixel(rasterPosY, &spriteX, color, collisionBuffer, spriteNrMask, doubleWidth);
                        } else
                            spriteX += doubleWidth ? 2 : 1;
                    }
                }
            }
        }
    }
}
static LARGE_INTEGER frequency;
static LARGE_INTEGER start, end;

void waitWithMultimediaTimer(double milliseconds) {
    timeBeginPeriod(1);  // Stellt die Timer-Auflösung auf 1 ms ein
    Sleep((DWORD)milliseconds);
    timeEndPeriod(1);    // Setzt die Timer-Auflösung wieder zurück
}

 
void updateVic(uint32_t clkCountS) {
    static uint32_t oldRaster;
    static int add=0;

    raster = (clkCountS / (63)) % PAL_B_MAX_RASTER;
    rasterXpos = clkCountS % (63);

    vicRegisters.rasterLine = raster & 0xff;
    vicRegisters.control1 = (vicRegisters.control1 & 0x7f) | ((raster >> 1) & 0x80);

    if (raster == vicRegisters.rasterCMP) {
#if 0
            uint8_t help = vicRegisters.borderColor;
            vicRegisters.borderColor = YELLOW;            
            drawBoarderLine(raster);
            vicRegisters.borderColor = help;
#endif
        if (vicRegisters.irqMask & 0x01) {
            vicRegisters.irqStatus = 0x81;
            doIRQ |= 1;
        }
    }

    if ((oldRaster != raster) && (rasterXpos > 40)) {

        busyWaitMicroseconds(50+add);
        startTsc = rdtsc();

        if ((raster >= 51) && (raster <= 250)) {
            drawCharLine(raster);
        }
        if ((raster >= 16) && (raster <= 299)) {
            updateSpriteLine(raster);
            drawBoarderLine(raster);
        }

        endTsc = rdtsc();
        uint64_t cntTsc = endTsc - startTsc;
        gesTsc = gesTsc + cntTsc;

        if (raster == (PAL_B_MAX_RASTER - 1)) {
            vicUpdateCnt++;
            windowsUpdateScreen(&windowsScreen[0][0]);
            QueryPerformanceFrequency(&frequency);  // Frequenz des Hochleistungszählers
            QueryPerformanceCounter(&end);  // Endzeitpunkt
            double elapsed = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
            QueryPerformanceCounter(&start);        // Startzeitpunkt
            if (vicUpdateCnt>1) {
                elapsed *= 1000;
                // printf("%3.3f\n", elapsed);
                if (elapsed<20) {
                    add++;
                   // waitWithMultimediaTimer(20 - elapsed);
                } else {
                    add--;
                }
            }    
        }
        oldRaster = raster;
    }
}

#if 0


#endif
