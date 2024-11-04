#include <stdio.h>
#include <stdlib.h>

void illegalOpcode(const char *opcode, const char *description) {
    printf("Illegal opcode %s (%s)\r\n", opcode, description);
    
}



void OPCODE_02(void) { illegalOpcode("0x02", "???"); }
void OPCODE_03(void) { illegalOpcode("0x03", "???"); }
void OPCODE_04(void) { illegalOpcode("0x04", "???"); }
void OPCODE_07(void) { illegalOpcode("0x07", "SLO Zero Page"); }
void OPCODE_0B(void) { illegalOpcode("0x0B", "ASL Accumulator"); }
void OPCODE_0C(void) { illegalOpcode("0x0C", "NOP (Absolute)"); }
void OPCODE_0F(void) { illegalOpcode("0x0F", "???"); }
void OPCODE_12(void) { illegalOpcode("0x12", "???"); }
void OPCODE_13(void) { illegalOpcode("0x13", "???"); }
void OPCODE_14(void) { illegalOpcode("0x14", "???"); }
void OPCODE_17(void) { illegalOpcode("0x17", "???"); }
void OPCODE_1A(void) { illegalOpcode("0x1A", "???"); }
void OPCODE_1B(void) { illegalOpcode("0x1B", "NOP"); }
void OPCODE_1C(void) { illegalOpcode("0x1C", "???"); }
void OPCODE_1F(void) { illegalOpcode("0x1F", "???"); }
void OPCODE_22(void) { illegalOpcode("0x22", "KIL"); }
void OPCODE_23(void) { illegalOpcode("0x23", "ASL Accumulator"); }
void OPCODE_27(void) { illegalOpcode("0x27", "RLA Zero Page"); }
void OPCODE_2B(void) { illegalOpcode("0x2B", "ASL Accumulator"); }
void OPCODE_2F(void) { illegalOpcode("0x2F", "???"); }
void OPCODE_32(void) { illegalOpcode("0x32", "KIL"); }
void OPCODE_33(void) { illegalOpcode("0x33", "NOP"); }
void OPCODE_34(void) { illegalOpcode("0x34", "NOP"); }
void OPCODE_37(void) { illegalOpcode("0x37", "NOP"); }
void OPCODE_3A(void) { illegalOpcode("0x3A", "NOP"); }
void OPCODE_3B(void) { illegalOpcode("0x3B", "NOP"); }
void OPCODE_3C(void) { illegalOpcode("0x3C", "NOP"); }
void OPCODE_3F(void) { illegalOpcode("0x3F", "NOP"); }
void OPCODE_42(void) { illegalOpcode("0x42", "KIL"); }
void OPCODE_43(void) { illegalOpcode("0x43", "NOP"); }
void OPCODE_44(void) { illegalOpcode("0x44", "NOP"); }
void OPCODE_47(void) { illegalOpcode("0x47", "SRE Zero Page"); }
void OPCODE_4B(void) { illegalOpcode("0x4B", "NOP"); }
void OPCODE_4F(void) { illegalOpcode("0x4F", "NOP"); }
void OPCODE_52(void) { illegalOpcode("0x52", "KIL"); }
void OPCODE_53(void) { illegalOpcode("0x53", "KIL"); }
void OPCODE_54(void) { illegalOpcode("0x54", "NOP"); }
void OPCODE_57(void) { illegalOpcode("0x57", "NOP"); }
void OPCODE_5C(void) { illegalOpcode("0x5C", "NOP"); }
void OPCODE_5B(void) { illegalOpcode("0x5B", "NOP"); }
void OPCODE_5F(void) { illegalOpcode("0x5F", "NOP"); }
void OPCODE_62(void) { illegalOpcode("0x62", "KIL"); }
void OPCODE_63(void) { illegalOpcode("0x63", "NOP"); }
void OPCODE_64(void) { illegalOpcode("0x64", "NOP"); }
void OPCODE_67(void) { illegalOpcode("0x67", "RRA Zero Page"); }
void OPCODE_6B(void) { illegalOpcode("0x6B", "NOP"); }
void OPCODE_6F(void) { illegalOpcode("0x6F", "NOP"); }
void OPCODE_72(void) { illegalOpcode("0x72", "NOP"); }
void OPCODE_73(void) { illegalOpcode("0x73", "NOP"); }
void OPCODE_74(void) { illegalOpcode("0x74", "NOP"); }
void OPCODE_77(void) { illegalOpcode("0x77", "NOP"); }
void OPCODE_7A(void) { illegalOpcode("0x7A", "NOP"); }
void OPCODE_7B(void) { illegalOpcode("0x7B", "NOP"); }
void OPCODE_7C(void) { illegalOpcode("0x7C", "NOP"); }
void OPCODE_7F(void) { illegalOpcode("0x7F", "NOP"); }
void OPCODE_80(void) { illegalOpcode("0x80", "NOP"); }
void OPCODE_82(void) { illegalOpcode("0x82", "NOP"); }
void OPCODE_83(void) { illegalOpcode("0x83", "NOP"); }
void OPCODE_87(void) { illegalOpcode("0x87", "NOP"); }
void OPCODE_89(void) { illegalOpcode("0x89", "NOP"); }
void OPCODE_8B(void) { illegalOpcode("0x8B", "NOP"); }
void OPCODE_8F(void) { illegalOpcode("0x8F", "NOP"); }
void OPCODE_92(void) { illegalOpcode("0x92", "KIL"); }
void OPCODE_93(void) { illegalOpcode("0x93", "TXS"); }
void OPCODE_97(void) { illegalOpcode("0x97", "TXS"); }
void OPCODE_9B(void) { illegalOpcode("0x9B", "TXS"); }
void OPCODE_9C(void) { illegalOpcode("0x9C", "NOP"); }
void OPCODE_9E(void) { illegalOpcode("0x9E", "NOP"); }
void OPCODE_9F(void) { illegalOpcode("0x9F", "NOP"); }
void OPCODE_A3(void) { illegalOpcode("0xA3", "SBC (Indirect), Y"); }
void OPCODE_A7(void) { illegalOpcode("0xA7", "LAX Zero Page"); }
void OPCODE_AB(void) { illegalOpcode("0xAB", "TSX"); }
void OPCODE_AF(void) { illegalOpcode("0xAF", "LAX Absolute"); }
void OPCODE_B2(void) { illegalOpcode("0xB2", "KIL"); }
void OPCODE_B3(void) { illegalOpcode("0xB3", "SBC (Indirect), Y"); }
void OPCODE_B7(void) { illegalOpcode("0xB7", "LAX Zero Page, Y"); }
void OPCODE_BB(void) { illegalOpcode("0xBB", "SBC Absolute, Y"); }
void OPCODE_BF(void) { illegalOpcode("0xBF", "SBC Absolute, Y"); }
void OPCODE_C2(void) { illegalOpcode("0xC2", "KIL"); }
void OPCODE_C3(void) { illegalOpcode("0xC3", "SBC (Indirect), Y"); }
void OPCODE_C7(void) { illegalOpcode("0xC7", "DCP Zero Page"); }
void OPCODE_CB(void) { illegalOpcode("0xCB", "SBC Absolute, Y"); }
void OPCODE_CF(void) { illegalOpcode("0xCF", "DCP Absolute"); }
void OPCODE_D2(void) { illegalOpcode("0xD2", "KIL"); }
void OPCODE_D3(void) { illegalOpcode("0xD3", "SBC (Indirect), Y"); }
void OPCODE_D4(void) { illegalOpcode("0xD4", "SBC (Indirect), Y"); }
void OPCODE_D7(void) { illegalOpcode("0xD7", "SBC Zero Page, Y"); }
void OPCODE_DA(void) { illegalOpcode("0xDA", "???"); }
void OPCODE_DB(void) { illegalOpcode("0xDB", "SBC Absolute, Y"); }
void OPCODE_DC(void) { illegalOpcode("0xDC", "SBC Absolute, Y"); }
void OPCODE_DF(void) { illegalOpcode("0xDF", "SBC Absolute, X"); }
void OPCODE_E2(void) { illegalOpcode("0xE2", "KIL"); }
void OPCODE_E3(void) { illegalOpcode("0xE3", "KIL"); }
void OPCODE_E7(void) { illegalOpcode("0xE7", "ISC Zero Page"); }
void OPCODE_EF(void) { illegalOpcode("0xEF", "ISC Absolute"); }
void OPCODE_EB(void) { illegalOpcode("0xEB", "???"); }
void OPCODE_F2(void) { illegalOpcode("0xF2", "KIL"); }
void OPCODE_F3(void) { illegalOpcode("0xF3", "KIL"); }
void OPCODE_F4(void) { illegalOpcode("0xF4", "ISC Zero Page, X"); }
void OPCODE_F7(void) { illegalOpcode("0xF7", "ISC Zero Page, X"); }
void OPCODE_FA(void) { illegalOpcode("0xFA", "ISC Zero Page, X"); }
void OPCODE_FB(void) { illegalOpcode("0xFb", "ISC Zero Page, X"); }
void OPCODE_FC(void) { illegalOpcode("0xFC", "ISC Zero Page, X"); }
void OPCODE_FF(void) { illegalOpcode("0xFF", "ISC Zero Page, X"); }


