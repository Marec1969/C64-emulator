#ifndef FLOPPY1570_H
#define FLOPPY1570_H

#include <stdint.h>
#include <stdbool.h>

// Simulierte Leitungen
extern bool serClk;
extern bool serDat;
extern bool serAtn;

// Globale Zeit in Mikrosekunden
extern uint32_t usTime;

// Gerät-Zustände
typedef enum {
    TRANSMIT_IDLE,           
    TRANSMIT_WAIT_FOR_DATA_1,
    TRANSMIT_WAIT_FOR_DATA_2,
    TRANSMIT_RECEIVING,        // Empfängt ein Byte
    TRANSMIT_ACKNOWLEDGE,       // Bestätigt Empfang
    TRANSMIT_EOI,               // Verarbeitet ein End-of-Indicator (EOI) Signal
    TRANSMIT_READY_FOR_DATA_1,
    TRANSMIT_READY_FOR_DATA_2,
    TRANSMIT_WAIT_ACK,
    TRANSMIT_SET_EOI,
    TRANSMIT_WAIT_EOI,
    TRANSMIT_RETURN_LISTEN,
    TRANSMIT_SENDING        // send a Byte
} TranmitState;


typedef enum {
    DEVICE_IDLE,           
    DEVICE_LISTEN,           
    DEVICE_TALK  
} DeviceState;

typedef enum {
    CHANNEL_OPEN,           
    CHANNEL_CLOSE           
} ChannelState;


#define TOTAL_TRACKS 70 // Maximale Anzahl der Spuren (1–70)

const uint8_t sectors_per_track[TOTAL_TRACKS] = {
    // Spur 01 bis 17: 21 Sektoren
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    // Spur 18 bis 24: 19 Sektoren
    19, 19, 19, 19, 19, 19, 19,
    // Spur 25 bis 30: 18 Sektoren
    18, 18, 18, 18, 18, 18,
    // Spur 31 bis 35: 17 Sektoren
    17, 17, 17, 17, 17,
    // Spur 36 bis 52: 21 Sektoren (nur 1571)
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    // Spur 53 bis 59: 19 Sektoren (nur 1571)
    19, 19, 19, 19, 19, 19, 19,
    // Spur 60 bis 65: 18 Sektoren (nur 1571)
    18, 18, 18, 18, 18, 18,
    // Spur 66 bis 70: 17 Sektoren (nur 1571)
    17, 17, 17, 17, 17
};

#define TRACK_COUNT 35       // Anzahl der Spuren
#define BLOCK_MAP_BYTES 3    // 3 Bytes für die Blockbelegung pro Spur
#define DISK_NAME_LENGTH 16
#define ID_LENGTH 2
#define FORMAT_ID_LENGTH 2
#define UNUSED_AREA_LENGTH (255 - 221 + 1) // Bereich von 180 bis 255

typedef struct {
    uint8_t directory_track;  // 000: Spurnummer für Directory
    uint8_t directory_sector; // 001: Startsektor für Directory
    uint8_t format_id;        // 002: Formatkennzeichen "A"
    uint8_t double_sided;     // 003: Flag für doppelseitige Disketten
    struct {
        uint8_t free_blocks;                  // Anzahl der freien Blöcke/Sektoren für die Spur
        uint8_t block_map[BLOCK_MAP_BYTES];   // Bitmuster der Blockbelegung (0 = belegt, 1 = frei)
    } track_data[TRACK_COUNT];                // 004-143: Daten für jede der 35 Spuren
    char disk_name[DISK_NAME_LENGTH]; // 144-159: Diskettenname
    uint8_t shift_space_1[2]; // 160-161: $a0 $a0 SHIFT SPACE
    char disk_id[ID_LENGTH];  // 162-163: ID der Diskette
    uint8_t shift_space_2;    // 164: $a0 SHIFT SPACE
    char format_spec[FORMAT_ID_LENGTH]; // 165-166: "2A"
    uint8_t shift_space_3[4]; // 167-170: $a0 $a0 $a0 $a0
    uint8_t mode_padding[9];  // 171-179: $00 oder $a0 (je nach Modus)
    uint8_t unused[UNUSED_AREA_LENGTH]; // 180-255: nicht verwendeter Bereich
} DiskStruct1570;

#define EXTRA_TRACK_COUNT 35 // Zusätzliche Spuren (36–70)

typedef struct {
    uint8_t directory_track;  // 000: Spurnummer für Directory
    uint8_t directory_sector; // 001: Startsektor für Directory
    uint8_t format_id;        // 002: Formatkennzeichen "A"
    uint8_t double_sided;     // 003: Flag für doppelseitige Disketten
    struct {
        uint8_t free_blocks;                  // Anzahl der freien Blöcke/Sektoren für die Spur
        uint8_t block_map[BLOCK_MAP_BYTES];   // Bitmuster der Blockbelegung (0 = belegt, 1 = frei)
    } track_data[TRACK_COUNT];                // 004-143: Daten für Spuren 1–35
    char disk_name[DISK_NAME_LENGTH];         // 144-159: Diskettenname
    uint8_t shift_space_1[2];                 // 160-161: $a0 $a0 SHIFT SPACE
    char disk_id[ID_LENGTH];                  // 162-163: ID der Diskette
    uint8_t shift_space_2;                    // 164: $a0 SHIFT SPACE
    char format_spec[FORMAT_ID_LENGTH];       // 165-166: "2A"
    uint8_t shift_space_3[4];                 // 167-170: $a0 $a0 $a0 $a0
    uint8_t mode_padding[9];                  // 171-179: $00 oder $a0 (je nach Modus)
    uint8_t unused[41];                       // 180-220: nicht verwendeter Bereich
    uint8_t extra_track_free_blocks[EXTRA_TRACK_COUNT]; // 221-255: Anzahl der freien Blöcke für Spuren 36–70
} DiskStruct1571;


#define FILENAME_LENGTH 16 // Maximale Länge des Dateinamens
#define NUM_DIRECTORY_ENTRIES 8 // Anzahl der Einträge pro Directory-Block

// Struktur eines Dateieintrags im Directory
typedef struct {
    uint8_t file_type;             // 000: Kennzeichen des Dateityps
    uint8_t start_track;           // 001: Spur des ersten Blocks der Datei
    uint8_t start_sector;          // 002: Sektor des ersten Blocks der Datei
    char filename[FILENAME_LENGTH]; // 003-018: Dateiname (mit $a0 aufgefüllt)
    uint8_t side_sector_track;     // 019: Spur des ersten Side-Sektor-Blocks (REL-Dateien)
    uint8_t side_sector_sector;    // 020: Sektor des ersten Side-Sektor-Blocks
    uint8_t record_length;         // 021: Recordlänge (nur REL-Dateien)
    uint8_t dos_buffer[4];         // 022-025: Zwischenspeicher für DOS-Operationen
    uint8_t replace_track;         // 026: Spur beim Überschreiben mit REPLACE
    uint8_t replace_sector;        // 027: Sektor beim Überschreiben mit REPLACE
    uint16_t block_count;          // 028-029: Anzahl der Blöcke der Datei (L/H)
} DirectoryEntry;

// Struktur eines Directory-Blocks
typedef struct {
    uint8_t next_track;            // 000: Spurnummer des nächsten Directoryblocks
    uint8_t next_sector;           // 001: Sektornummer des nächsten Directoryblocks
    DirectoryEntry entries[NUM_DIRECTORY_ENTRIES]; // 002-255: Einträge für 8 Dateien
} DirectoryBlock;


#define MAX_SIDE_SECTOR_BLOCKS 6   // Maximale Anzahl von Side-Sektor-Blöcken (0–5)
#define RECORD_DIRECTORY_ENTRIES 120 // Maximale Anzahl von Datensätzen

typedef struct {
    uint8_t next_track;            // 000: Spurnummer des nächsten Side-Sektor-Blocks
    uint8_t next_sector;           // 001: Sektornummer des nächsten Side-Sektor-Blocks
    uint8_t block_number;          // 002: Nummer dieses Side-Sektor-Blocks (0 bis 5)
    uint8_t record_length;         // 003: Recordlänge in der Datei
    struct {
        uint8_t track;             // Spur eines angelegten Side-Sektor-Blocks
        uint8_t sector;            // Sektor eines angelegten Side-Sektor-Blocks
    } side_sectors[MAX_SIDE_SECTOR_BLOCKS]; // 004-015: Spur- und Sektornummern der Side-Sektor-Blöcke
    struct {
        uint8_t track;             // Spur eines Datensatzes
        uint8_t sector;            // Sektor eines Datensatzes
    } record_directory[RECORD_DIRECTORY_ENTRIES]; // 016-255: Spur- und Sektornummern der Datensätze
} SideSectorBlock;

#include <stdint.h>

#define MAX_PROGRAM_SIZE 255 // Maximale Größe eines Programms in Bytes

typedef struct {
    uint8_t next_track;             // 000: Spurnummer des Folgeblocks
    uint8_t next_sector;            // 001: Sektornummer des Folgeblocks
    uint16_t start_address;         // 002-003: Startadresse des Programms (ADL/ADH)
    uint8_t program_size;           // 004: Anzahl der Bytes des Programms (maximal 255)
    uint8_t program_data[MAX_PROGRAM_SIZE]; // 005-...: Eigentliche Programmdaten
    uint8_t checksum;               // x+1: Prüfsumme über den vorigen Programmteil
} ProgramBlock;

typedef struct {
    uint16_t next_start_address;    // x+2/x+3: Startadresse des nächsten Programms im Speicher
    uint8_t next_program_size;      // x+4: Anzahl der Bytes des nächsten Programms (maximal 255)
    uint8_t next_program_data[MAX_PROGRAM_SIZE]; // x+5-...: Eigentliche Programmdaten des nächsten Programms
    uint8_t next_checksum;          // y+1: Prüfsumme des nächsten Programmteils
} NextProgram;

void updateDataline(void);
void device1570Update(void);
void init1570(void);

#endif
