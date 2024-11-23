

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "cia.h"
#include "ciaRtc.h"
#include "cpu6510.h"
#include "cpurunner.h"
#include "vic.h"

#include "floppy1570.h"

#define CIA2_Data       0x20
#define CIA2_Clock      0x10
#define CIA2_Attention  0x08

bool serClkC64 = true;
bool serDatC64 = true;


CIA cia2;
static uint16_t ciaTimer_ctrl = 0;

void writeCia2(uint16_t adresse, uint8_t value) {
    switch (adresse) {
        case 0xDD00:  // Adresse: 0xDD00 (CIA 2)
                      // Port A Data Register
                      // Bit 7-0: I/O Pins von Port A
            //printf("write to cia2 PAR %02x \n",value);
            cia2.pra = value;  
            if (cia2.ddra & CIA2_Data) {
                serDatC64 = ((cia2.pra & CIA2_Data) !=0);
            }
            if (cia2.ddra & CIA2_Clock) {
                serClkC64 = ((cia2.pra & CIA2_Clock) !=0);
            }
            if (cia2.ddra & CIA2_Attention) {
                serAtn = ((cia2.pra & CIA2_Attention) !=0);
            }
            updateDataline();
            break;

        case 0xDD01:  // Adresse: 0xDD01 (CIA 2)
                      // Port B Data Register
                      // Bit 7-0: I/O Pins von Port B
            cia2.prb = value;
            break;

        case 0xDD02:  // Adresse: 0xDD02 (CIA 2)
                      // Data Direction Register A
                      // Bit 7-0: 0 = Input, 1 = Output für Port A
            // printf("write cia2  direction %02x\n",value);
            cia2.pra |=  ~value;  // die Eingänge nehmen durch den Pull-UP eien "1" an wenn sie auch Eingang porgrammiert werden
            cia2.ddra = value;
            break;

        case 0xDD03:  // Adresse: 0xDD03 (CIA 2)
                      // Data Direction Register B
                      // Bit 7-0: 0 = Input, 1 = Output für Port B
            cia2.prb =
                ~value;  // die Eingänge nehmen durch den Pull-UP eien "1" an wenn sie auch Eingang porgrammiert werden
            cia2.ddrb = value;
            break;

        case 0xDD04:  // Adresse: 0xDD04 (CIA 2)
                      // Timer A, Low Byte
            cia2.reloadTimerA = (cia2.reloadTimerA & 0xff00) | value;
            // cia2.timerA_lo = value;
            break;

        case 0xDD05:  // Adresse: 0xDD05 (CIA 2)
                      // Timer A, High Byte
                      // 16-bit Timer A (frei programmierbar, Countdown)
            cia2.reloadTimerA = (cia2.reloadTimerA & 0x00ff) | (value << 8);
            // cia2.timerA_hi = value;
            break;

        case 0xDD06:  // Adresse: 0xDD06 (CIA 2)
                      // Timer B, Low Byte
            cia2.reloadTimerB = (cia2.reloadTimerB & 0xff00) | value;
            // cia2.timerB_lo = value;
            break;

        case 0xDD07:  // Adresse: 0xDD07 (CIA 2)
                      // Timer B, High Byte
                      // 16-bit Timer B (frei programmierbar, Countdown)
            cia2.reloadTimerB = (cia2.reloadTimerB & 0x00ff) | (value << 8);
            // cia2.timerB_hi = value;
            break;

        case 0xDD08:
        case 0xDD09:
        case 0xDD0A:
        case 0xDD0B:
            write_regRTC(&cia2RTC, adresse, value);
            break;

        case 0xDD0C:  // Adresse: 0xDD0C (CIA 2)
                      // Serial Data Register (SDR)
                      // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation
            cia2.sdr = value;
            break;

        case 0xDD0D:  // Adresse: 0xDD0D (CIA 2)
                      // Interrupt Control Register (ICR)
                      // Bit 7: Setzt Interrupt-Flag
                      // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                      // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                      // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                      // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das
                      // serielle Schieberegister. Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke
                      // am FLAG-Pin.
            if (value & 0x80) {
                cia2.icrMask |= value;
            } else {
                cia2.icrMask &= ~value;
            }
            break;

        case 0xDD0E:  // Adresse: 0xDD0E (CIA 2)
                      // Control Register A (CRA)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 6 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 6 invertiert , 1 = Bei Timer-Unterlauf
                      // wird an Port B das Bit 6 für einen Systemtaktzyklus High Bit 3: 0 = Timer-Neustart nach
                      // Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf Bit 4: 1 = Einmalig Latch
                      // in den Timer laden Bit 5: 0 = Timer wird mit der Systemfrequenz getaktet, 1 = Timer wird von
                      // positiven Flanken am CNT-Pin getaktet Bit 6: Richtung des seriellen Schieberegisters, 0 =
                      // SP-Pin ist Eingang (lesen), 1 = SP-Pin ist Ausgang (schreiben) Bit 7: Echtzeituhr, 0 = 60 Hz, 1
                      // = 50 Hz an Pin 19
            if (value & 0x01) {
                ciaTimer_ctrl |= TIMERA2_RUN;
            } else {
                ciaTimer_ctrl &= ~TIMERA2_RUN;
            }
            if (value & 0x08) {
                ciaTimer_ctrl &= ~TIMERA2_RELOAD;
            } else {
                ciaTimer_ctrl |= TIMERA2_RELOAD;
            }
            if (value & 0x10) {
                cia2.timerA = cia2.reloadTimerA;
            }
            if (value & 0xE6) {
                printf("CIA2 Timer A  (0xdc0e) not implemented %02x\n ", value);
                setException(EXCEPTION_CIA);
            }
            cia2.cra = value;
            break;

        case 0xDD0F:  // Adresse: 0xDD0F (CIA 2)
                      // Control Register B (CRB)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 7 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 7 invertiert , 1 = Bei Timer-Unterlauf
                      // wird an Port B das Bit 7 für einen Systemtaktzyklus High Bit 3: 0 = Timer-Neustart nach
                      // Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf Bit 4: 1 = Einmalig Latch
                      // in den Timer laden Bit 5..6: Timer wird getaktet ... %00 = mit der Systemfrequenz %01 = von
                      // positiver Flanke am CNT-Pin %10 = vom Unterlauf des Timer A %11 = vom Unterlauf des Timer A,
                      // wenn CNT-Pin High ist Bit 7: 0 = Schreiben in die TOD-Register setzt die Uhrzeit, 1 = Schreiben
                      // in die TOD-Register setzt die Alarmzeit
            if (value & 0x01) {
                ciaTimer_ctrl |= TIMERB2_RUN;
            } else {
                ciaTimer_ctrl &= ~TIMERB2_RUN;
            }
            if (value & 0x08) {
                ciaTimer_ctrl &= ~TIMERB2_RELOAD;
            } else {
                ciaTimer_ctrl |= TIMERB2_RELOAD;
            }
            if (value & 0x10) {
                cia2.timerB = cia2.reloadTimerB;
            }
            if (value & 0xE6) {
                printf("CIA2 Timer B  (0xdc0f) not implemented %02x\n ", value);
                setException(EXCEPTION_CIA);
            }
            cia2.crb = value;
            break;

        default:
            // Ungültige Adresse
            break;
    }
}

uint8_t readCia2(uint16_t adresse) {
    uint8_t value;

    switch (adresse) {
        case 0xDD00:  // Adresse: 0xDD00 (CIA 2)
                      // Port A Data Register
                      // Bit 7-0: I/O Pins von Port A
            // printf("read from cia2 PAR %02x\n",cia2.pra);

            value = cia2.pra;

            if (serDat) {
                value = value & ~0x80;   
            } else {
                value = value | 0x80;   
            }

            if (serClk) {
                value = value & ~0x40;   
            } else {
                value = value | 0x40;   
            }

            break;

        case 0xDD01:  // Adresse: 0xDD01 (CIA 2)
                      // Port B Data Register
                      // Bit 7-0: I/O Pins von Port B
            value = cia2.prb;
            break;

        case 0xDD02:  // Adresse: 0xDD02 (CIA 2)
                      // Data Direction Register A
                      // Bit 7-0: 0 = Input, 1 = Output für Port A
            value = cia2.ddra;
            break;

        case 0xDD03:  // Adresse: 0xDD03 (CIA 2)
                      // Data Direction Register B
                      // Bit 7-0: 0 = Input, 1 = Output für Port B
            value = cia2.ddrb;
            break;

        case 0xDD04:  // Adresse: 0xDD04 (CIA 2)
                      // Timer A, Low Byte
            value = cia2.timerA & 0xff;
            break;

        case 0xDD05:  // Adresse: 0xDD05 (CIA 2)
                      // Timer A, High Byte
                      // 16-bit Timer A (frei programmierbar, Countdown)
            value = (cia2.timerA >> 8) & 0xff;
            break;

        case 0xDD06:  // Adresse: 0xDD06 (CIA 2)
                      // Timer B, Low Byte
            value = (cia2.timerB >> 8) & 0xff;
            break;

        case 0xDD07:  // Adresse: 0xDD07 (CIA 2)
                      // Timer B, High Byte
                      // 16-bit Timer B (frei programmierbar, Countdown)
            value = (cia2.timerB >> 8) & 0xff;
            break;

        case 0xDD08:
        case 0xDD09:
        case 0xDD0A:
        case 0xDD0B:
            value = read_regRTC(&cia2RTC, adresse);
            break;

        case 0xDD0C:  // Adresse: 0xDD0C (CIA 2)
                      // Serial Data Register (SDR)
                      // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation
            value = cia2.sdr;
            break;

        case 0xDD0D:  // Adresse: 0xDD0D (CIA 2)
                      // Interrupt Control Register (ICR)
                      // Bit 7: Setzt Interrupt-Flag
                      // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                      // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                      // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                      // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das
                      // serielle Schieberegister. Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke
                      // am FLAG-Pin.
            value = cia2.icr;
            cia2.icr = 0;
            doNIM &= ~(CIA2_A_NIM | CIA2_B_NIM);
            break;

        case 0xDD0E:  // Adresse: 0xDD0E (CIA 2)
                      // Control Register A (CRA)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 6 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 6 invertiert , 1 = Bei Timer-Unterlauf
                      // wird an Port B das Bit 6 für einen Systemtaktzyklus High Bit 3: 0 = Timer-Neustart nach
                      // Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf Bit 4: 1 = Einmalig Latch
                      // in den Timer laden Bit 5: 0 = Timer wird mit der Systemfrequenz getaktet, 1 = Timer wird von
                      // positiven Flanken am CNT-Pin getaktet Bit 6: Richtung des seriellen Schieberegisters, 0 =
                      // SP-Pin ist Eingang (lesen), 1 = SP-Pin ist Ausgang (schreiben) Bit 7: Echtzeituhr, 0 = 60 Hz, 1
                      // = 50 Hz an Pin 19
            value = cia2.cra;
            break;

        case 0xDD0F:  // Adresse: 0xDD0F (CIA 2)
                      // Control Register B (CRB)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 7 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 7 invertiert , 1 = Bei Timer-Unterlauf
                      // wird an Port B das Bit 7 für einen Systemtaktzyklus High Bit 3: 0 = Timer-Neustart nach
                      // Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf Bit 4: 1 = Einmalig Latch
                      // in den Timer laden Bit 5..6: Timer wird getaktet ... %00 = mit der Systemfrequenz %01 = von
                      // positiver Flanke am CNT-Pin %10 = vom Unterlauf des Timer A %11 = vom Unterlauf des Timer A,
                      // wenn CNT-Pin High ist Bit 7: 0 = Schreiben in die TOD-Register setzt die Uhrzeit, 1 = Schreiben
                      // in die TOD-Register setzt die Alarmzeit
            value = cia2.crb;
            break;

        default:
            // Ungültige Adresse
            break;
    }
    return value;
}

#define TENS_OF_SEC 100000

void updateCia2(uint8_t clkCount) {
    static int32_t sClkCount = 0;

    sClkCount = sClkCount + clkCount;
    if (sClkCount >= TENS_OF_SEC) {
        sClkCount = sClkCount - TENS_OF_SEC;
        increment_rtc(&cia2RTC);
    }

    if (ciaTimer_ctrl) {
        if (ciaTimer_ctrl & (TIMERA2_RUN)) {
            cia2.timerA -= clkCount;
            if (cia2.timerA <= 0) {
                if (cia2.icrMask & 0x01) {
                    doNIM |= CIA2_A_NIM;
                    cia2.icr |= 0x81;
                } else {
                    cia2.icr |= 0x01;  // set underrun                     
                }
                if (ciaTimer_ctrl & TIMERA2_RELOAD) {
                    cia2.timerA = cia2.reloadTimerA;
                } else {
                    cia2.timerA = 0;
                    ciaTimer_ctrl &= ~TIMERA2_RUN;
                }
            }
        }
        if (ciaTimer_ctrl & (TIMERB2_RUN)) {
            cia2.timerB -= clkCount;
            if (cia2.timerB <= 0) {
                if (cia2.icrMask & 0x02) {
                    doNIM |= CIA2_B_NIM;
                    cia2.icr |= 0x82;
                } else {
                    cia2.icr |= 0x02;  // set underrun                     
                }
                if (ciaTimer_ctrl & TIMERB2_RELOAD) {
                    cia2.timerB = cia2.reloadTimerB;
                } else {
                    cia2.timerB = 0;
                    ciaTimer_ctrl &= ~TIMERB2_RUN;
                }
            }
        }
    }
}

uint8_t ciaGetvidoebank(void) {
    uint8_t bank;
    bank = (~cia2.pra) & 0x03;
    return (bank);
}

void writeCia2toTxtFile(void) { writeCiatoFile("cia2_register_info.txt", &cia2, &cia2RTC); }
