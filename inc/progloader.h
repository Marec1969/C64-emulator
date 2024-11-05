#ifndef PROGLOADER_H
#define PROGLOADER_H

typedef struct {
    uint8_t signature[32];
    uint16_t version;
    uint16_t maxItems;
    uint16_t realItems;
    uint16_t reserved;
    uint8_t container[24];
} t64Head_t;

typedef struct {
    uint8_t type;
    uint8_t typeAdd;
    uint16_t startAddr;
    uint16_t endAddr;
    uint16_t reserved1;
    uint32_t offset;
    uint16_t reserved2;
    uint16_t reserved3;
    uint8_t name[16];
} t64Cunk_t;

void loadPrg(int rawKey);
#endif