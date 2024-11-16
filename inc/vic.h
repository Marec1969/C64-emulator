
#ifndef VIC_H
#define VIC_H

#include <stdint.h>

#define VICADDR 0xD000
#define VICEND  0xD3FF
#define VICMASK 0xD03F


#define BLACK 0
#define WHITE 1
#define RED 2
#define TURQUOISE 3
#define PURPLE 4
#define GREEN 5
#define BLUE 6
#define YELLOW 7
#define ORANGE 8
#define BROWN 9
#define LIGHT_RED 10
#define DARK_GRAY 11
#define MEDIUM_GRAY 12
#define LIGHT_GREEN 13
#define LIGHT_BLUE 14
#define LIGHT_GRAY 15

#define COLOR_ADDR 0xD800
#define COLOR_ADDR_END 0xDBFF

#define MULTICOLORTEXT 0x10
#define EXTENDEDBACKCOLOR 0x40

#define INDEX_X 0
#define INDEX_Y 1

#define SPRITE_Y_SIZE 21
#define SPRITE_X_SIZE 24
#define SPRITE_X_BYTE (SPRITE_X_SIZE / 8)

#define SCREEN_WIDTH 320
#define SCREEN_HEIGT 200

#define SCREENSIZE_BYTE 0x400
#define MAX_SPRITES 8

// RSEL	Höhe des Anzeigefensters 	Erste Zeile   	Letzte Zeile
// 0	24 Textzeilen/192 Pixel     	55 ($37)	    246 ($f6)
// 1	25 Textzeilen/200 Pixel 	    51 ($33)	    250 ($fa)
//
// CSEL	Breite des Anzeigefensters	Erste X-Koo.	Letzte X-Koo.
// 0	38 Zeichen/304 Pixel	        31 ($1f	        334 ($14e)
// 1	40 Zeichen/320 Pixel	        24 ($18)	    343 ($157)

// Typ	    	Videonorm	Anzahl Zeilen	Sichtbare Zeilen	Zyklen/Zeile	Sichtbare Pixel/Zeile
// 6567R56A	    NTSC-M	        262	            234	                64	            411
// 6567R8	    NTSC-M	        263	            235	                65	            418
// 6569	        PAL-B	        312	            284             	63	            403

#define PAL_B_MAX_RASTER 312

#define PAL_B_Y 312
#define PAL_B_X 400

// Typ	            Erste   Letzte          Erste X-Koo.    Erste sichtbare     Letzte sichtbare
//          V-Blank-Zeile	V-Blank-Zeile	einer Zeile     X-Koordinate        X-Koordinate
// 6567R56A	        13	        40	            412 ($19c)	    488 ($1e8)      	388 ($184)
// 6567R8	        13	        40	            412 ($19c)  	489 ($1e9)	        396 ($18c)
// 6569	           300	        15	            404 ($194)  	480 ($1e0)	        380 ($17c)

// Der Referenzpunkt für den Beginn einer Rasterzeile ist das Auftreten des Raster-IRQ,
// der aber nicht mit der X-Koordinate 0 zusammenfällt, sondern mit der unter „Erste X-Koo.
// einer Zeile” angegebenen. Die X-Koordinaten laufen innerhalb der Zeile bis $1ff (beim 6569 nur bis $1f7),
// dann erst kommt X-Koordinate 0.

#define VISIBLE_SPRITE_POS_X 24
#define VISIBLE_SPRITE_POS_Y 50

#define SPRITE_OFFSET_X (BR_LEFT - VISIBLE_SPRITE_POS_X)
#define SPRITE_OFFSET_Y (BR_TOP - VISIBLE_SPRITE_POS_Y)

// #define VIC_LINES_CNT   312
#define VIC_VIB_LINE 300
#define VIC_MAX_X 343
#define VIC_MAX_Y 312

#define BR_LEFT (40)
#define BR_TOP (51)
#define BR_RIGHT (400 - 40)
#define BR_BOTTOM (SCREEN_HEIGT + BR_TOP)

#define C64BITMAP (0x20)

typedef struct {
    // $D000: Sprite 0 X-Position (siehe auch $D010)
    // $D001: Sprite 0 Y-Position
    uint8_t spritePos[MAX_SPRITES][2];

    // $D010: Höchstes Bit der X-Position für alle Sprites (Bit-Nr. = Sprite-Nr.)
    // Bit 0: Sprite 0 X-MSB
    // Bit 1: Sprite 1 X-MSB
    // Bit 2: Sprite 2 X-MSB
    // Bit 3: Sprite 3 X-MSB
    // Bit 4: Sprite 4 X-MSB
    // Bit 5: Sprite 5 X-MSB
    // Bit 6: Sprite 6 X-MSB
    // Bit 7: Sprite 7 X-MSB
    uint8_t spriteX_msb;

    // $D011: Kontrollregister 1
    // Bit 7: 9. Bit der Rasterzeile ($D012)
    // Bit 6: Extended Color Modus
    // Bit 5: Bitmap-Modus
    // Bit 4: Bildausgabe an/aus (aktiviert mit nächstem Bild)
    // Bit 3: 25 Zeilen (1) oder 24 Zeilen (0)
    // Bit 2-0: Rasterzeilen-Offset vom oberen Rand
    uint8_t control1;

    // $D012: Rasterzeile (IRQ wird hier ausgelöst)
    uint8_t rasterLine;

    // $D013: Lichtgriffel X-Position (Lesen)
    uint8_t lightpenX;

    // $D014: Lichtgriffel Y-Position (Lesen)
    uint8_t lightpenY;

    // $D015: Sprite Aktivierung (Bit-Nr. = Sprite-Nr.)
    // Bit 0: Sprite 0 aktiv
    // Bit 1: Sprite 1 aktiv
    // Bit 2: Sprite 2 aktiv
    // Bit 3: Sprite 3 aktiv
    // Bit 4: Sprite 4 aktiv
    // Bit 5: Sprite 5 aktiv
    // Bit 6: Sprite 6 aktiv
    // Bit 7: Sprite 7 aktiv
    uint8_t spriteEnable;

    // $D016: Kontrollregister 2
    // Bit 7-5: unbenutzt
    // Bit 4: Multicolor-Modus für Text und Grafik (nicht für Sprites)
    // Bit 3: 40 Spalten (1) oder 38 Spalten (0)
    // Bit 2-0: Pixel-Offset vom linken Rand
    uint8_t control2;

    // $D017: Sprites mit doppelter Höhe (Bit-Nr. = Sprite-Nr.)
    // Bit 0: Sprite 0 doppelte Höhe
    // Bit 1: Sprite 1 doppelte Höhe
    // Bit 2: Sprite 2 doppelte Höhe
    // Bit 3: Sprite 3 doppelte Höhe
    // Bit 4: Sprite 4 doppelte Höhe
    // Bit 5: Sprite 5 doppelte Höhe
    // Bit 6: Sprite 6 doppelte Höhe
    // Bit 7: Sprite 7 doppelte Höhe
    uint8_t spriteDouble_height;

    // $D018: Speicherkontrollregister
    // Bit 7-4: Adresse des Bildschirmspeichers für den Textmodus (1024 * (Bits 7-4))
    // Bit 3-1: Adresse des Zeichensatzes für den Textmodus (2048 * (Bits 3-1))
    // Bit 3: Im Bitmap-Modus wählt dies den Speicherbereich für Grafik (0 = $0000-$1FFF, 1 = $2000-$3FFF)
    // Bit 0: wird überlesen
    uint8_t memoryControl;

    // $D019: Interrupt-Status (lesen: 1 = IRQ aufgetreten, schreiben: IRQ bestätigen)
    // Bit 7: IRQ aufgetreten (welcher steht in Bits 3-0)
    // Bit 3: IRQ von Lichtgriffel
    // Bit 2: IRQ durch Sprite-Sprite-Kollision
    // Bit 1: IRQ durch Sprite-Hintergrund-Kollision
    // Bit 0: IRQ von Rasterzeile
    uint8_t irqStatus;

    // $D01A: Interrupt-Maske (IRQ freigeben)
    // Bit 3: Lichtgriffel IRQ erlauben
    // Bit 2: Sprite-Sprite-Kollision IRQ erlauben
    // Bit 1: Sprite-Hintergrund-Kollision IRQ erlauben
    // Bit 0: Rasterzeile IRQ erlauben
    uint8_t irqMask;

    // $D01B: Sprite Priorität (0 = vor Hintergrund, 1 = hinter Hintergrund) (Bit-Nr. = Sprite-Nr.)
    uint8_t spriteBackground_priority;

    // $D01C: Sprite Multicolor-Modus (0 = Hi-Res, 1 = Multicolor) (Bit-Nr. = Sprite-Nr.)
    uint8_t spriteMulticolor;

    // $D01D: Sprite doppelte Breite (0 = normale Breite, 1 = doppelte Breite) (Bit-Nr. = Sprite-Nr.)
    uint8_t spriteDouble_width;

    // $D01E: Sprite-Sprite-Kollisionen (1 = Kollision) (Bit-Nr. = Sprite-Nr.)
    uint8_t spriteCollision;

    // $D01F: Sprite-Hintergrund-Kollisionen (1 = Kollision) (Bit-Nr. = Sprite-Nr.)
    uint8_t spriteBackground_collision;

    // $D020: Rahmenfarbe
    uint8_t borderColor;

    // $D021: Hintergrundfarbe
    uint8_t backgroundColor;

    // $D022: Hintergrundfarbe 1 (für Extended Color-Modus, siehe $D011)
    uint8_t backgroundColor1;

    // $D023: Hintergrundfarbe 2 (für Extended Color-Modus, siehe $D011)
    uint8_t backgroundColor2;

    // $D024: Hintergrundfarbe 3 (für Extended Color-Modus, siehe $D011)
    uint8_t backgroundColor3;

    // $D025: Erste Spritefarbe (Multicolor_0 für Sprite-Multicolormodus, siehe $D01C)
    uint8_t spriteMulticolor0;

    // $D026: Zweite Spritefarbe (Multicolor_1 für Sprite-Multicolormodus, siehe $D01C)
    uint8_t spriteMulticolor1;

    // $D027 - $D02E: Farbe der Sprites 0-7
    uint8_t spriteColor[8];

    // $D02F: Status der zusätzlichen Tastatur-Pins (nur C128)
    // Bit 2-0: Status der Tastatur-Pins
    uint8_t keyboardPins;

    // $D030: Betriebsmodus
    // Bit 1: Testmodus
    // Bit 0: 1 MHz (0) oder 2 MHz (1)
    uint8_t mode;

    uint16_t rasterCMP;

    uint8_t padding[15];

} VIC_II_Registers;

// Structure for colors with color value and RGB values
struct Farbe {
    uint8_t colorValue;  // Color value 1..15
    uint8_t r;           // Red
    uint8_t g;           // Green
    uint8_t b;           // Blue
};

extern uint8_t colormap[1024];

extern VIC_II_Registers vicRegisters;

extern int saveScreen();
extern uint8_t readVic(uint16_t addr);
extern void writeVic_registers_to_file(void);
extern void writeVic(uint16_t addr, uint8_t value);
extern void updateVic(uint32_t clkCount);
extern void keyMapDown(int ascii, int rawKey);
extern void keyMapUp(int rawKey);
#endif