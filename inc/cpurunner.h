
#include "opcodes.h"
#include <stdio.h>

typedef struct {
    const char* m1;
    const char* m2;
} Opcode;


#ifdef DOINCLUDE_CPUDATA

static Opcode opcodes[256] = {
      {"BRK", ""}, {"ORA (", ",X)"}, {"KIL", ""}, {"SLO (", ",X)"},
    {"NOP", ""}, {"ORA ", ""}, {"ASL ", ""}, {"SLO ", ""},
    {"PHP", ""}, {"ORA #", ""}, {"ASL ", "A"}, {"ANC #", ""},
    {"NOP", ""}, {"ORA ", ""}, {"ASL ", ""}, {"SLO ", ""},
    
    {"BPL ", ""}, {"ORA (", "),Y"}, {"KIL", ""}, {"SLO (", "),Y"},
    {"NOP", ""}, {"ORA ", ",X"}, {"ASL ", ",X"}, {"SLO ", ",X"},
    {"CLC", ""}, {"ORA ", ",Y"}, {"NOP", ""}, {"SLO ", ",Y"},
    {"NOP", ""}, {"ORA ", ",X"}, {"ASL ", ",X"}, {"SLO ", ",X"},
    
    {"JSR ", ""}, {"AND (", ",X)"}, {"KIL", ""}, {"RLA (", ",X)"},
    {"BIT ", ""}, {"AND ", ""}, {"ROL ", ""}, {"RLA ", ""},
    {"PLP", ""}, {"AND #", ""}, {"ROL ", "A"}, {"ANC #", ""},
    {"BIT ", ""}, {"AND ", ""}, {"ROL ", ""}, {"RLA ", ""},
    
    {"BMI ", ""}, {"AND (", "),Y"}, {"KIL", ""}, {"RLA (", "),Y"},
    {"NOP", ""}, {"AND ", ",X"}, {"ROL ", ",X"}, {"RLA ", ",X"},
    {"SEC", ""}, {"AND ", ",Y"}, {"NOP", ""}, {"RLA ", ",Y"},
    {"NOP", ""}, {"AND ", ",X"}, {"ROL ", ",X"}, {"RLA ", ",X"},
    
    {"RTI", ""}, {"EOR (", ",X)"}, {"KIL", ""}, {"SRE (", ",X)"},
    {"NOP", ""}, {"EOR ", ""}, {"LSR ", ""}, {"SRE ", ""},
    {"PHA", ""}, {"EOR #", ""}, {"LSR ", "A"}, {"ALR #", ""},
    {"JMP ", ""}, {"EOR ", ""}, {"LSR ", ""}, {"SRE ", ""},
    
    {"BVC ", ""}, {"EOR (", "),Y"}, {"KIL", ""}, {"SRE (", "),Y"},
    {"NOP", ""}, {"EOR ", ",X"}, {"LSR ", ",X"}, {"SRE ", ",X"},
    {"CLI", ""}, {"EOR ", ",Y"}, {"NOP", ""}, {"SRE ", ",Y"},
    {"NOP", ""}, {"EOR ", ",X"}, {"LSR ", ",X"}, {"SRE ", ",X"},
    
    {"RTS", ""}, {"ADC (", ",X)"}, {"KIL", ""}, {"RRA (", ",X)"},
    {"NOP", ""}, {"ADC ", ""}, {"ROR ", ""}, {"RRA ", ""},
    {"PLA", ""}, {"ADC #", ""}, {"ROR ", "A"}, {"ARR #", ""},
    {"JMP ", ""}, {"ADC ", ""}, {"ROR ", ""}, {"RRA ", ""},
    
    {"BVS ", ""}, {"ADC (", "),Y"}, {"KIL", ""}, {"RRA (", "),Y"},
    {"NOP", ""}, {"ADC ", ",X"}, {"ROR ", ",X"}, {"RRA ", ",X"},
    {"SEI", ""}, {"ADC ", ",Y"}, {"NOP", ""}, {"RRA ", ",Y"},
    {"NOP", ""}, {"ADC ", ",X"}, {"ROR ", ",X"}, {"RRA ", ",X"},
    
    {"NOP", ""}, {"STA (", ",X)"}, {"NOP", ""}, {"SAX (", ",X)"},
    {"STY ", ""}, {"STA ", ""}, {"STX ", ""}, {"SAX ", ""},
    {"DEY", ""}, {"NOP", ""}, {"TXA", ""}, {"XAA #", ""},
    {"STY ", ""}, {"STA ", ""}, {"STX ", ""}, {"SAX ", ""},
    
    {"BCC ", ""}, {"STA (", "),Y"}, {"KIL", ""}, {"AHX (", "),Y"},
    {"STY ", ",X"}, {"STA ", ",X"}, {"STX ", ",Y"}, {"SAX ", ",Y"},
    {"TYA", ""}, {"STA ", ",Y"}, {"TXS", ""}, {"TAS", ""},
    {"SHY ", ",X"}, {"STA ", ",X"}, {"SHX ", ",X"}, {"AHX ", ""},
    
    {"LDY #", ""}, {"LDA (", ",X)"}, {"LDX #", ""}, {"LAX (", ",X)"},
    {"LDY ", ""}, {"LDA ", ""}, {"LDX ", ""}, {"LAX ", ""},
    {"TAY", ""}, {"LDA #", ""}, {"TAX", ""}, {"LAX #", ""},
    {"LDY ", ""}, {"LDA ", ""}, {"LDX ", ""}, {"LAX ", ",Y"},
    
    {"BCS ", ""}, {"LDA (", "),Y"}, {"KIL", ""}, {"LAX (", "),Y"},
    {"LDY ", ",X"}, {"LDA ", ",X"}, {"LDX ", ",Y"}, {"LAX ", ",Y"},
    {"CLV", ""}, {"LDA ", ",Y"}, {"TSX", ""}, {"LAS ", ""},
    {"LDY ", ""}, {"LDA ", ",X"}, {"LDX ", ""}, {"LAX ", ",Y"},
    
    {"CPY #", ""}, {"CMP (", ",X)"}, {"NOP", ""}, {"DCP (", ",X)"},
    {"CPY ", ""}, {"CMP ", ""}, {"DEC ", ""}, {"DCP ", ""},
    {"INY", ""}, {"CMP #", ""}, {"DEX", ""}, {"AXS #", ""},
    {"CPY ", ""}, {"CMP ", ""}, {"DEC ", ""}, {"DCP ", ",X"},
    
    {"BNE ", ""}, {"CMP (", "),Y"}, {"KIL", ""}, {"DCP (", "),Y"},
    {"NOP", ""}, {"CMP ", ",X"}, {"DEC ", ",X"}, {"DCP ", ",X"},
    {"CLD", ""}, {"CMP ", ",Y"}, {"NOP", ""}, {"DCP ", ",Y"},
    {"NOP", ""}, {"CMP ", ",X"}, {"DEC ", ",X"}, {"DCP ", ",X"},
    
    {"CPX #", ""}, {"SBC (", ",X)"}, {"NOP", ""}, {"ISC (", ",X)"},
    {"CPX ", ""}, {"SBC ", ""}, {"INC ", ""}, {"ISC ", ""},
    {"INX", ""}, {"SBC #", ""}, {"NOP", ""}, {"SBC #", ""},
    {"CPX ", ""}, {"SBC ", ""}, {"INC ", ""}, {"ISC ", ",X"},
    
    {"BEQ ", ""}, {"SBC (", "),Y"}, {"KIL", ""}, {"ISC (", "),Y"},
    {"NOP", ""}, {"SBC ", ",X"}, {"INC ", ",X"}, {"ISC ", ",X"},
    {"SED", ""}, {"SBC ", ",Y"}, {"NOP", ""}, {"ISC ", ",Y"},
    {"NOP", ""}, {"SBC ", ",X"}, {"INC ", ""}, {"ISC ", ",X"}
};

static const int opcodeCycles[256]= {
    7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0, 
    2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0, 
    2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0, 
    2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0, 
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0
};



 static const int opcodeLengths[256] = {
        1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0,  // 00-0F
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,  // 10-1F
        3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,  // 20-2F
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,  // 30-3F
        1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,  // 40-4F
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,  // 50-5F
        1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,  // 60-6F
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,  // 70-7F
        0, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,  // 80-8F
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,  // 90-9F
        2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,  // A0-AF
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,  // B0-BF
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,  // C0-CF
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,  // D0-DF
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,  // E0-EF
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0   // F0-FF
    };

// Sprungtabelle (Jumptable)
static OPCODE_func jumptable[256] = {
    /* 0x00 */ OPCODE_00, /* 0x01 */ OPCODE_01, /* 0x02 */ OPCODE_02, /* 0x03 */ OPCODE_03,
    /* 0x04 */ OPCODE_04, /* 0x05 */ OPCODE_05, /* 0x06 */ OPCODE_06, /* 0x07 */ OPCODE_07,
    /* 0x08 */ OPCODE_08, /* 0x09 */ OPCODE_09, /* 0x0A */ OPCODE_0A, /* 0x0B */ OPCODE_0B,
    /* 0x0C */ OPCODE_0C, /* 0x0D */ OPCODE_0D, /* 0x0E */ OPCODE_0E, /* 0x0F */ OPCODE_0F,
    /* 0x10 */ OPCODE_10, /* 0x11 */ OPCODE_11, /* 0x12 */ OPCODE_12, /* 0x13 */ OPCODE_13,
    /* 0x14 */ OPCODE_14, /* 0x15 */ OPCODE_15, /* 0x16 */ OPCODE_16, /* 0x17 */ OPCODE_17,
    /* 0x18 */ OPCODE_18, /* 0x19 */ OPCODE_19, /* 0x1A */ OPCODE_1A, /* 0x1B */ OPCODE_1B,
    /* 0x1C */ OPCODE_1C, /* 0x1D */ OPCODE_1D, /* 0x1E */ OPCODE_1E, /* 0x1F */ OPCODE_1F,
    /* 0x20 */ OPCODE_20, /* 0x21 */ OPCODE_21, /* 0x22 */ OPCODE_22, /* 0x23 */ OPCODE_23,
    /* 0x24 */ OPCODE_24, /* 0x25 */ OPCODE_25, /* 0x26 */ OPCODE_26, /* 0x27 */ OPCODE_27,
    /* 0x28 */ OPCODE_28, /* 0x29 */ OPCODE_29, /* 0x2A */ OPCODE_2A, /* 0x2B */ OPCODE_2B,
    /* 0x2C */ OPCODE_2C, /* 0x2D */ OPCODE_2D, /* 0x2E */ OPCODE_2E, /* 0x2F */ OPCODE_2F,
    /* 0x30 */ OPCODE_30, /* 0x31 */ OPCODE_31, /* 0x32 */ OPCODE_32, /* 0x33 */ OPCODE_33,
    /* 0x34 */ OPCODE_34, /* 0x35 */ OPCODE_35, /* 0x36 */ OPCODE_36, /* 0x37 */ OPCODE_37,
    /* 0x38 */ OPCODE_38, /* 0x39 */ OPCODE_39, /* 0x3A */ OPCODE_3A, /* 0x3B */ OPCODE_3B,
    /* 0x3C */ OPCODE_3C, /* 0x3D */ OPCODE_3D, /* 0x3E */ OPCODE_3E, /* 0x3F */ OPCODE_3F,
    /* 0x40 */ OPCODE_40, /* 0x41 */ OPCODE_41, /* 0x42 */ OPCODE_42, /* 0x43 */ OPCODE_43,
    /* 0x44 */ OPCODE_44, /* 0x45 */ OPCODE_45, /* 0x46 */ OPCODE_46, /* 0x47 */ OPCODE_47,
    /* 0x48 */ OPCODE_48, /* 0x49 */ OPCODE_49, /* 0x4A */ OPCODE_4A, /* 0x4B */ OPCODE_4B,
    /* 0x4C */ OPCODE_4C, /* 0x4D */ OPCODE_4D, /* 0x4E */ OPCODE_4E, /* 0x4F */ OPCODE_4F,
    /* 0x50 */ OPCODE_50, /* 0x51 */ OPCODE_51, /* 0x52 */ OPCODE_52, /* 0x53 */ OPCODE_53,
    /* 0x54 */ OPCODE_54, /* 0x55 */ OPCODE_55, /* 0x56 */ OPCODE_56, /* 0x57 */ OPCODE_57,
    /* 0x58 */ OPCODE_58, /* 0x59 */ OPCODE_59, /* 0x5A */ OPCODE_5A, /* 0x5B */ OPCODE_5B,
    /* 0x5C */ OPCODE_5C, /* 0x5D */ OPCODE_5D, /* 0x5E */ OPCODE_5E, /* 0x5F */ OPCODE_5F,
    /* 0x60 */ OPCODE_60, /* 0x61 */ OPCODE_61, /* 0x62 */ OPCODE_62, /* 0x63 */ OPCODE_63,
    /* 0x64 */ OPCODE_64, /* 0x65 */ OPCODE_65, /* 0x66 */ OPCODE_66, /* 0x67 */ OPCODE_67,
    /* 0x68 */ OPCODE_68, /* 0x69 */ OPCODE_69, /* 0x6A */ OPCODE_6A, /* 0x6B */ OPCODE_6B,
    /* 0x6C */ OPCODE_6C, /* 0x6D */ OPCODE_6D, /* 0x6E */ OPCODE_6E, /* 0x6F */ OPCODE_6F,
    /* 0x70 */ OPCODE_70, /* 0x71 */ OPCODE_71, /* 0x72 */ OPCODE_72, /* 0x73 */ OPCODE_73,
    /* 0x74 */ OPCODE_74, /* 0x75 */ OPCODE_75, /* 0x76 */ OPCODE_76, /* 0x77 */ OPCODE_77,
    /* 0x78 */ OPCODE_78, /* 0x79 */ OPCODE_79, /* 0x7A */ OPCODE_7A, /* 0x7B */ OPCODE_7B,
    /* 0x7C */ OPCODE_7C, /* 0x7D */ OPCODE_7D, /* 0x7E */ OPCODE_7E, /* 0x7F */ OPCODE_7F,
    /* 0x80 */ OPCODE_80, /* 0x81 */ OPCODE_81, /* 0x82 */ OPCODE_82, /* 0x83 */ OPCODE_83,
    /* 0x84 */ OPCODE_84, /* 0x85 */ OPCODE_85, /* 0x86 */ OPCODE_86, /* 0x87 */ OPCODE_87,
    /* 0x88 */ OPCODE_88, /* 0x89 */ OPCODE_89, /* 0x8A */ OPCODE_8A, /* 0x8B */ OPCODE_8B,
    /* 0x8C */ OPCODE_8C, /* 0x8D */ OPCODE_8D, /* 0x8E */ OPCODE_8E, /* 0x8F */ OPCODE_8F,
    /* 0x90 */ OPCODE_90, /* 0x91 */ OPCODE_91, /* 0x92 */ OPCODE_92, /* 0x93 */ OPCODE_93,
    /* 0x94 */ OPCODE_94, /* 0x95 */ OPCODE_95, /* 0x96 */ OPCODE_96, /* 0x97 */ OPCODE_97,
    /* 0x98 */ OPCODE_98, /* 0x99 */ OPCODE_99, /* 0x9A */ OPCODE_9A, /* 0x9B */ OPCODE_9B,
    /* 0x9C */ OPCODE_9C, /* 0x9D */ OPCODE_9D, /* 0x9E */ OPCODE_9E, /* 0x9F */ OPCODE_9F,
    /* 0xA0 */ OPCODE_A0, /* 0xA1 */ OPCODE_A1, /* 0xA2 */ OPCODE_A2, /* 0xA3 */ OPCODE_A3,
    /* 0xA4 */ OPCODE_A4, /* 0xA5 */ OPCODE_A5, /* 0xA6 */ OPCODE_A6, /* 0xA7 */ OPCODE_A7,
    /* 0xA8 */ OPCODE_A8, /* 0xA9 */ OPCODE_A9, /* 0xAA */ OPCODE_AA, /* 0xAB */ OPCODE_AB,
    /* 0xAC */ OPCODE_AC, /* 0xAD */ OPCODE_AD, /* 0xAE */ OPCODE_AE, /* 0xAF */ OPCODE_AF,
    /* 0xB0 */ OPCODE_B0, /* 0xB1 */ OPCODE_B1, /* 0xB2 */ OPCODE_B2, /* 0xB3 */ OPCODE_B3,
    /* 0xB4 */ OPCODE_B4, /* 0xB5 */ OPCODE_B5, /* 0xB6 */ OPCODE_B6, /* 0xB7 */ OPCODE_B7,
    /* 0xB8 */ OPCODE_B8, /* 0xB9 */ OPCODE_B9, /* 0xBA */ OPCODE_BA, /* 0xBB */ OPCODE_BB,
    /* 0xBC */ OPCODE_BC, /* 0xBD */ OPCODE_BD, /* 0xBE */ OPCODE_BE, /* 0xBF */ OPCODE_BF,
    /* 0xC0 */ OPCODE_C0, /* 0xC1 */ OPCODE_C1, /* 0xC2 */ OPCODE_C2, /* 0xC3 */ OPCODE_C3,
    /* 0xC4 */ OPCODE_C4, /* 0xC5 */ OPCODE_C5, /* 0xC6 */ OPCODE_C6, /* 0xC7 */ OPCODE_C7,
    /* 0xC8 */ OPCODE_C8, /* 0xC9 */ OPCODE_C9, /* 0xCA */ OPCODE_CA, /* 0xCB */ OPCODE_CB,
    /* 0xCC */ OPCODE_CC, /* 0xCD */ OPCODE_CD, /* 0xCE */ OPCODE_CE, /* 0xCF */ OPCODE_CF,
    /* 0xD0 */ OPCODE_D0, /* 0xD1 */ OPCODE_D1, /* 0xD2 */ OPCODE_D2, /* 0xD3 */ OPCODE_D3,
    /* 0xD4 */ OPCODE_D4, /* 0xD5 */ OPCODE_D5, /* 0xD6 */ OPCODE_D6, /* 0xD7 */ OPCODE_D7,
    /* 0xD8 */ OPCODE_D8, /* 0xD9 */ OPCODE_D9, /* 0xDA */ OPCODE_DA, /* 0xDB */ OPCODE_DB,
    /* 0xDC */ OPCODE_DC, /* 0xDD */ OPCODE_DD, /* 0xDE */ OPCODE_DE, /* 0xDF */ OPCODE_DF,
    /* 0xE0 */ OPCODE_E0, /* 0xE1 */ OPCODE_E1, /* 0xE2 */ OPCODE_E2, /* 0xE3 */ OPCODE_E3,
    /* 0xE4 */ OPCODE_E4, /* 0xE5 */ OPCODE_E5, /* 0xE6 */ OPCODE_E6, /* 0xE7 */ OPCODE_E7,
    /* 0xE8 */ OPCODE_E8, /* 0xE9 */ OPCODE_E9, /* 0xEA */ OPCODE_EA, /* 0xEB */ OPCODE_EB,
    /* 0xEC */ OPCODE_EC, /* 0xED */ OPCODE_ED, /* 0xEE */ OPCODE_EE, /* 0xEF */ OPCODE_EF,
    /* 0xF0 */ OPCODE_F0, /* 0xF1 */ OPCODE_F1, /* 0xF2 */ OPCODE_F2, /* 0xF3 */ OPCODE_F3,
    /* 0xF4 */ OPCODE_F4, /* 0xF5 */ OPCODE_F5, /* 0xF6 */ OPCODE_F6, /* 0xF7 */ OPCODE_F7,
    /* 0xF8 */ OPCODE_F8, /* 0xF9 */ OPCODE_F9, /* 0xFA */ OPCODE_FA, /* 0xFB */ OPCODE_FB,
    /* 0xFC */ OPCODE_FC, /* 0xFD */ OPCODE_FD, /* 0xFE */ OPCODE_FE, /* 0xFF */ OPCODE_FF,
};
#endif

#ifndef CPURUNNER_H
#define CPURUNNER_H

#define EXCEPTION_WRITE_MEM 1
#define EXCEPTION_OPCODE 2
#define EXCEPTION_ILEGAL 3
#define EXCEPTION_CIA 4
#define EXCEPTION_VCI 5
#define EXCEPTION_IRQ 6
#define EXCEPTION_TRACE 7
#define EXCEPTION_READ_MEM 8

void cpuRunnerInit(void);
void cpuRunnerDo(void);
void setException(int16_t from);

extern int16_t doIRQ;
extern int16_t doNIM;


#endif