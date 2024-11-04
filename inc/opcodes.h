// Header-Datei: opcodes.h

#ifndef OPCODES_H
#define OPCODES_H

extern void OPCODE_00(void);  // BRK
extern void OPCODE_01(void);  // ORA (Indirect,cpu.X)
extern void OPCODE_02(void);  // NOP (Illegal Opcode)
extern void OPCODE_03(void);  // SLO (Illegal Opcode)
extern void OPCODE_04(void);  // NOP Zero Page
extern void OPCODE_05(void);  // ORA Zero Page
extern void OPCODE_06(void);  // ASL Zero Page
extern void OPCODE_07(void);  // ROL Zero Page
extern void OPCODE_08(void);  // PHP
extern void OPCODE_09(void);  // ORA Immediate
extern void OPCODE_0A(void);  // ASL Accumulator
extern void OPCODE_0B(void);  // ANC (Illegal Opcode)
extern void OPCODE_0C(void);  // NOP Absolute
extern void OPCODE_0D(void);  // ORA Absolute
extern void OPCODE_0E(void);  // ASL Absolute
extern void OPCODE_0F(void);  // SLO (Illegal Opcode)
extern void OPCODE_10(void);  // BPL (Branch if Positive)
extern void OPCODE_11(void);  // ORA (Indirect),Y
extern void OPCODE_12(void);  // NOP (Illegal Opcode)
extern void OPCODE_13(void);  // SLO (Illegal Opcode)
extern void OPCODE_14(void);  // NOP Zero Page,X
extern void OPCODE_15(void);  // ORA Zero Page,cpu.X
extern void OPCODE_16(void);  // ASL Zero Page,cpu.X
extern void OPCODE_17(void);  // ROL Zero Page,Y
extern void OPCODE_18(void);  // CLC
extern void OPCODE_19(void);  // ORA Absolute,Y
extern void OPCODE_1A(void);  // NOP
extern void OPCODE_1B(void);  // SLO (Illegal Opcode)
extern void OPCODE_1C(void);  // NOP Absolute,X
extern void OPCODE_1D(void);  // ORA Absolute,cpu.X
extern void OPCODE_1E(void);  // ASL Absolute,cpu.X
extern void OPCODE_1F(void);  // SLO (Illegal Opcode)
extern void OPCODE_20(void);  // JSR Absolute
extern void OPCODE_21(void);  // AND (Indirect,cpu.X)
extern void OPCODE_22(void);  // NOP (Illegal Opcode)
extern void OPCODE_23(void);  // RLA (Illegal Opcode)
extern void OPCODE_24(void);  // BIT Zero Page
extern void OPCODE_25(void);  // AND Zero Page
extern void OPCODE_26(void);  // ROL Zero Page
extern void OPCODE_27(void);  // ROR Zero Page
extern void OPCODE_28(void);  // PLP
extern void OPCODE_29(void);  // AND Immediate
extern void OPCODE_2A(void);  // ROL Accumulator
extern void OPCODE_2B(void);  // ANC (Illegal Opcode)
extern void OPCODE_2C(void);  // BIT Absolute
extern void OPCODE_2D(void);  // AND Absolute
extern void OPCODE_2E(void);  // ROL Absolute
extern void OPCODE_2F(void);  // RLA (Illegal Opcode)
extern void OPCODE_30(void);  // BMI (Branch if Minus)
extern void OPCODE_31(void);  // AND (Indirect),Y
extern void OPCODE_32(void);  // NOP (Illegal Opcode)
extern void OPCODE_33(void);  // RLA (Illegal Opcode)
extern void OPCODE_34(void);  // NOP Zero Page,X
extern void OPCODE_35(void);  // AND Zero Page,cpu.X
extern void OPCODE_36(void);  // ROL Zero Page,cpu.X
extern void OPCODE_37(void);  // ROR Zero Page,Y
extern void OPCODE_38(void);  // SEC
extern void OPCODE_39(void);  // AND Absolute,Y
extern void OPCODE_3A(void);  // NOP
extern void OPCODE_3B(void);  // RLA (Illegal Opcode)
extern void OPCODE_3C(void);  // NOP Absolute,X
extern void OPCODE_3D(void);  // AND Absolute,cpu.X
extern void OPCODE_3E(void);  // ROL Absolute,cpu.X
extern void OPCODE_3F(void);  // RLA (Illegal Opcode)
extern void OPCODE_40(void);  // RTI (Return from Interrupt)
extern void OPCODE_41(void);  // EOR (Indirect,cpu.X)
extern void OPCODE_42(void);  // NOP (Illegal Opcode)
extern void OPCODE_43(void);  // SRE (Illegal Opcode)
extern void OPCODE_44(void);  // NOP Zero Page
extern void OPCODE_45(void);  // EOR Zero Page
extern void OPCODE_46(void);  // LSR Zero Page
extern void OPCODE_47(void);  // ROR Zero Page
extern void OPCODE_48(void);  // PHA
extern void OPCODE_49(void);  // EOR Immediate
extern void OPCODE_4A(void);  // LSR Accumulator
extern void OPCODE_4B(void);  // ALR (Illegal Opcode)
extern void OPCODE_4C(void);  // JMP Absolute
extern void OPCODE_4D(void);  // EOR Absolute
extern void OPCODE_4E(void);  // LSR Absolute
extern void OPCODE_4F(void);  // SRE (Illegal Opcode)
extern void OPCODE_50(void);  // BVC (Branch if Overflow Clear)
extern void OPCODE_51(void);  // EOR (Indirect),Y
extern void OPCODE_52(void);  // NOP (Illegal Opcode)
extern void OPCODE_53(void);  // SRE (Illegal Opcode)
extern void OPCODE_54(void);  // NOP Zero Page,X
extern void OPCODE_55(void);  // EOR Zero Page,cpu.X
extern void OPCODE_56(void);  // LSR Zero Page,cpu.X
extern void OPCODE_57(void);  // ROR Zero Page,Y
extern void OPCODE_58(void);  // CLI
extern void OPCODE_59(void);  // EOR Absolute,Y
extern void OPCODE_5A(void);  // NOP
extern void OPCODE_5B(void);  // SRE (Illegal Opcode)
extern void OPCODE_5C(void);  // NOP Absolute,X
extern void OPCODE_5D(void);  // EOR Absolute,cpu.X
extern void OPCODE_5E(void);  // LSR Absolute,cpu.X
extern void OPCODE_5F(void);  // SRE (Illegal Opcode)
extern void OPCODE_60(void);  // RTS (Return from Subroutine)
extern void OPCODE_61(void);  // ADC (Indirect,cpu.X)
extern void OPCODE_62(void);  // NOP (Illegal Opcode)
extern void OPCODE_63(void);  // RLA (Illegal Opcode)
extern void OPCODE_64(void);  // NOP Zero Page
extern void OPCODE_65(void);  // ADC Zero Page
extern void OPCODE_66(void);  // ROR Zero Page
extern void OPCODE_67(void);  // ROR Zero Page
extern void OPCODE_68(void);  // PLA
extern void OPCODE_69(void);  // ADC Immediate
extern void OPCODE_6A(void);  // ROR Accumulator
extern void OPCODE_6B(void);  // ARR (Illegal Opcode)
extern void OPCODE_6C(void);  // JMP (Indirect)
extern void OPCODE_6D(void);  // ADC Absolute
extern void OPCODE_6E(void);  // ROR Absolute
extern void OPCODE_6F(void);  // RLA (Illegal Opcode)
extern void OPCODE_70(void);  // BVS (Branch if Overflow Set)
extern void OPCODE_71(void);  // ADC (Indirect),Y
extern void OPCODE_72(void);  // NOP (Illegal Opcode)
extern void OPCODE_73(void);  // RLA (Illegal Opcode)
extern void OPCODE_74(void);  // NOP Zero Page,X
extern void OPCODE_75(void);  // ADC Zero Page,cpu.X
extern void OPCODE_76(void);  // ROR Zero Page,cpu.X
extern void OPCODE_77(void);  // RLA (Illegal Opcode)
extern void OPCODE_78(void);  // SEI
extern void OPCODE_79(void);  // ADC Absolute,Y
extern void OPCODE_7A(void);  // NOP
extern void OPCODE_7B(void);  // RLA (Illegal Opcode)
extern void OPCODE_7C(void);  // NOP Absolute,X
extern void OPCODE_7D(void);  // ADC Absolute,cpu.X
extern void OPCODE_7E(void);  // ROR Absolute,cpu.X
extern void OPCODE_7F(void);  // RLA (Illegal Opcode)
extern void OPCODE_80(void);  // NOP (Illegal Opcode)
extern void OPCODE_81(void);  // STA (Indirect,cpu.X)
extern void OPCODE_82(void);  // NOP (Illegal Opcode)
extern void OPCODE_83(void);  // AHX (Illegal Opcode)
extern void OPCODE_84(void);  // STY Zero Page
extern void OPCODE_85(void);  // STA Zero Page
extern void OPCODE_86(void);  // STX Zero Page
extern void OPCODE_87(void);  // SAX Zero Page
extern void OPCODE_88(void);  // DEY
extern void OPCODE_89(void);  // NOP (Illegal Opcode)
extern void OPCODE_8A(void);  // TXA
extern void OPCODE_8B(void);  // XAA (Illegal Opcode)
extern void OPCODE_8C(void);  // STY Absolute
extern void OPCODE_8D(void);  // STA Absolute
extern void OPCODE_8E(void);  // STX Absolute
extern void OPCODE_8F(void);  // SAX Absolute
extern void OPCODE_90(void);  // BCC (Branch if Carry Clear)
extern void OPCODE_91(void);  // STA (Indirect),Y
extern void OPCODE_92(void);  // NOP (Illegal Opcode)
extern void OPCODE_93(void);  // AHX (Illegal Opcode)
extern void OPCODE_94(void);  // STY Zero Page,X
extern void OPCODE_95(void);  // STA Zero Page,cpu.X
extern void OPCODE_96(void);  // STX Zero Page,Y
extern void OPCODE_97(void);  // SAX Zero Page,Y
extern void OPCODE_98(void);  // TYA
extern void OPCODE_99(void);  // STA Absolute,Y
extern void OPCODE_9A(void);  // TXS
extern void OPCODE_9B(void);  // TAS (Illegal Opcode)
extern void OPCODE_9C(void);  // SHY (Illegal Opcode)
extern void OPCODE_9D(void);  // STA Absolute,X
extern void OPCODE_9E(void);  // SHY (Illegal Opcode)
extern void OPCODE_9F(void);  // AHX (Illegal Opcode)
extern void OPCODE_A0(void);  // LDY Immediate
extern void OPCODE_A1(void);  // LDA (Indirect,cpu.X)
extern void OPCODE_A2(void);  // LDX Immediate
extern void OPCODE_A3(void);  // LAX (Illegal Opcode)
extern void OPCODE_A4(void);  // LDY Zero Page
extern void OPCODE_A5(void);  // LDA Zero Page
extern void OPCODE_A6(void);  // LDX Zero Page
extern void OPCODE_A7(void);  // LAX Zero Page
extern void OPCODE_A8(void);  // TAY
extern void OPCODE_A9(void);  // LDA Immediate
extern void OPCODE_AA(void);  // TAX
extern void OPCODE_AB(void);  // LAX (Illegal Opcode)
extern void OPCODE_AC(void);  // LDY Absolute
extern void OPCODE_AD(void);  // LDA Absolute
extern void OPCODE_AE(void);  // LDX Absolute
extern void OPCODE_AF(void);  // LAX Absolute
extern void OPCODE_B0(void);  // BCS (Branch if Carry Set)
extern void OPCODE_B1(void);  // LDA (Indirect),Y
extern void OPCODE_B2(void);  // NOP (Illegal Opcode)
extern void OPCODE_B3(void);  // LAX (Illegal Opcode)
extern void OPCODE_B4(void);  // LDY Zero Page,X
extern void OPCODE_B5(void);  // LDA Zero Page,cpu.X
extern void OPCODE_B6(void);  // LDX Zero Page,Y
extern void OPCODE_B7(void);  // LAX Zero Page,Y
extern void OPCODE_B8(void);  // CLV
extern void OPCODE_B9(void);  // LDA Absolute,Y
extern void OPCODE_BA(void);  // TSX
extern void OPCODE_BB(void);  // LAS (Illegal Opcode)
extern void OPCODE_BC(void);  // LDY Absolute,X
extern void OPCODE_BD(void);  // LDA Absolute,cpu.X
extern void OPCODE_BE(void);  // LDX Absolute,Y
extern void OPCODE_BF(void);  // LAX Absolute,Y
extern void OPCODE_C0(void);  // CPY Immediate
extern void OPCODE_C1(void);  // CMP (Indirect,cpu.X)
extern void OPCODE_C2(void);  // NOP (Illegal Opcode)
extern void OPCODE_C3(void);  // DCP (Illegal Opcode)
extern void OPCODE_C4(void);  // CPY Zero Page
extern void OPCODE_C5(void);  // CMP Zero Page
extern void OPCODE_C6(void);  // DEC Zero Page
extern void OPCODE_C7(void);  // DCP Zero Page
extern void OPCODE_C8(void);  // INY
extern void OPCODE_C9(void);  // CMP Immediate
extern void OPCODE_CA(void);  // DEX
extern void OPCODE_CB(void);  // AXS (Illegal Opcode)
extern void OPCODE_CC(void);  // CPY Absolute
extern void OPCODE_CD(void);  // CMP Absolute
extern void OPCODE_CE(void);  // DEC Absolute
extern void OPCODE_CF(void);  // DCP Absolute
extern void OPCODE_D0(void);  // BNE (Branch if Not Equal)
extern void OPCODE_D1(void);  // CMP (Indirect),Y
extern void OPCODE_D2(void);  // NOP (Illegal Opcode)
extern void OPCODE_D3(void);  // DCP (Illegal Opcode)
extern void OPCODE_D4(void);  // NOP Zero Page,X
extern void OPCODE_D5(void);  // CMP Zero Page,cpu.X
extern void OPCODE_D6(void);  // DEC Zero Page,cpu.X
extern void OPCODE_D7(void);  // DCP (Illegal Opcode)
extern void OPCODE_D8(void);  // CLD
extern void OPCODE_D9(void);  // CMP Absolute,Y
extern void OPCODE_DA(void);  // NOP
extern void OPCODE_DB(void);  // DCP (Illegal Opcode)
extern void OPCODE_DC(void);  // NOP Absolute,X
extern void OPCODE_DD(void);  // CMP Absolute,cpu.X
extern void OPCODE_DE(void);  // DEC Absolute,cpu.X
extern void OPCODE_DF(void);  // DCP (Illegal Opcode)
extern void OPCODE_E0(void);  // CPX Immediate
extern void OPCODE_E1(void);  // SBC (Indirect,cpu.X)
extern void OPCODE_E2(void);  // NOP (Illegal Opcode)
extern void OPCODE_E3(void);  // ISC (Illegal Opcode)
extern void OPCODE_E4(void);  // CPX Zero Page
extern void OPCODE_E5(void);  // SBC Zero Page
extern void OPCODE_E6(void);  // INC Zero Page
extern void OPCODE_E7(void);  // ISC (Illegal Opcode)
extern void OPCODE_E8(void);  // INX
extern void OPCODE_E9(void);  // SBC Immediate
extern void OPCODE_EA(void);  // NOP
extern void OPCODE_EB(void);  // SBC Immediate (Illegal Opcode)
extern void OPCODE_EC(void);  // CPX Absolute
extern void OPCODE_ED(void);  // SBC Absolute
extern void OPCODE_EE(void);  // INC Absolute
extern void OPCODE_EF(void);  // ISC (Illegal Opcode)
extern void OPCODE_F0(void);  // BEQ (Branch if Equal)
extern void OPCODE_F1(void);  // SBC (Indirect),Y
extern void OPCODE_F2(void);  // NOP (Illegal Opcode)
extern void OPCODE_F3(void);  // SBC (Indirect,X)
extern void OPCODE_F4(void);  // NOP Zero Page,Y (Illegal Opcode)
extern void OPCODE_F5(void);  // SBC Zero Page,cpu.X
extern void OPCODE_F6(void);  // INC Zero Page,cpu.X
extern void OPCODE_F7(void);  // SBC (Illegal Opcode)
extern void OPCODE_F8(void);  // SED
extern void OPCODE_F9(void);  // SBC Absolute,Y
extern void OPCODE_FA(void);  // NOP
extern void OPCODE_FB(void);  // SBC (Illegal Opcode)
extern void OPCODE_FC(void);  // NOP Absolute,X
extern void OPCODE_FD(void);  // SBC Absolute,cpu.X
extern void OPCODE_FE(void);  // INC Absolute,cpu.X
extern void OPCODE_FF(void);  // SBC (Illegal Opcode)

#endif // OPCODES_H
