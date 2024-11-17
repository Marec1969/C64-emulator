

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "cia.h"
#include "ciaRtc.h"
#include "cpu6510.h"
#include "cpurunner.h"
#include "vic.h"

CIA cia1;
portKeyMap_t portKeyMap;
const uint8_t portNrToBitMap[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
static uint16_t ciaTimer_ctrl = 0;

void writeCia1(uint16_t adresse, uint8_t value) {
    switch (adresse) {
        case 0xDC00:  // Adresse: 0xDC00 (CIA 1)
                      // Port A Data Register
                      // Bit 7-0: I/O Pins von Port A
                      // Angeschlossen als Spannngsausgang für Tastatur
            cia1.pra = value;
            // printf("Write A %02X \n",value);
            break;

        case 0xDC01:  // Adresse: 0xDC01 (CIA 1)
                      // Port B Data Register
                      // Bit 7-0: I/O Pins von Port B
                      // Angeschlossen als Spannngseingang für Tastatur
            cia1.prb = value;
            break;

        case 0xDC02:  // Adresse: 0xDC02 (CIA 1)
                      // Data Direction Register A
                      // Bit 7-0: 0 = Input, 1 = Output für Port A
            cia1.ddra = value;
            cia1.pra |=
                ~value;  // die Eingänge nehmen durch den Pull-UP eien "1" an wenn sie auch Eingang porgrammiert werden
            break;

        case 0xDC03:  // Adresse: 0xDC03 (CIA 1)
                      // Data Direction Register B
                      // Bit 7-0: 0 = Input, 1 = Output für Port B
            cia1.ddrb = value;
            cia1.prb =
                ~value;  // die Eingänge nehmen durch den Pull-UP eien "1" an wenn sie auch Eingang porgrammiert werden
            break;

        case 0xDC04:  // Adresse: 0xDC04 (CIA 1)
                      // Timer A, Low Byte
            cia1.reloadTimerA = (cia1.reloadTimerA & 0xff00) | value;
            // cia1.timerA_lo = value;
            break;

        case 0xDC05:  // Adresse: 0xDC05 (CIA 1)
                      // Timer A, High Byte
                      // 16-bit Timer A (frei programmierbar, Countdown)
            cia1.reloadTimerA = (cia1.reloadTimerA & 0x00ff) | (value << 8);
            // cia1.timerA_hi = value;
            break;

        case 0xDC06:  // Adresse: 0xDC06 (CIA 1)
                      // Timer B, Low Byte
            cia1.reloadTimerB = (cia1.reloadTimerB & 0xff00) | value;
            // cia1.timerB_lo = value;
            break;

        case 0xDC07:  // Adresse: 0xDC07 (CIA 1)
                      // Timer B, High Byte
                      // 16-bit Timer B (frei programmierbar, Countdown)
            cia1.reloadTimerB = (cia1.reloadTimerB & 0x00ff) | (value << 8);
            // cia1.timerB_hi = value;
            break;

        case 0xDC08:
        case 0xDC09:
        case 0xDC0A:
        case 0xDC0B:
            write_regRTC(&cia1RTC, adresse, value);
            break;

        case 0xDC0C:  // Adresse: 0xDC0C (CIA 1)
                      // Serial Data Register (SDR)
                      // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation
            cia1.sdr = value;
            break;

        case 0xDC0D:  // Adresse: 0xDC0D (CIA 1)
                      // Interrupt Control Register (ICR)
                      // Bit 7: Setzt Interrupt-Flag
                      // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                      // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                      // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                      // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das
                      // serielle Schieberegister. Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke
                      // am FLAG-Pin.
            // printf("Write CAI 1 ICR %02x\n",value);
            if (value & 0x80) {
                cia1.icrMask |= value;
            } else {
                cia1.icrMask &= ~value;
            }
            break;

        case 0xDC0E:  // Adresse: 0xDC0E (CIA 1)
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
                ciaTimer_ctrl |= TIMERA1_RUN;
            } else {
                ciaTimer_ctrl &= ~TIMERA1_RUN;
            }
            if (value & 0x08) {
                ciaTimer_ctrl &= ~TIMERA1_RELOAD;
            } else {
                ciaTimer_ctrl |= TIMERA1_RELOAD;
            }
            if (value & 0x10) {
                cia1.timerA = cia1.reloadTimerA;
            }
            if (value & 0xE6) {
                printf("CIA1 Timer A  (0xdc0e) not implemented %02x\n ", value);
            }
            cia1.cra = value;
            break;

        case 0xDC0F:  // Adresse: 0xDC0F (CIA 1)
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
                ciaTimer_ctrl |= TIMERB1_RUN;
            } else {
                ciaTimer_ctrl &= ~TIMERB1_RUN;
            }

            if (value & 0x08) {
                ciaTimer_ctrl &= ~TIMERB1_RELOAD;
            } else {
                ciaTimer_ctrl |= TIMERB1_RELOAD;
            }
            if (value & 0x10) {
                cia1.timerB = cia1.reloadTimerB;
            }
            if (value & 0xE6) {
                printf("CIA1 Timer B  (0xdc0f) not implemented %02x\n ", value);
                setException(EXCEPTION_CIA);
            }
            cia1.crb = value;
            break;

        default:
            // Ungültige Adresse
            break;
    }
}

uint8_t readCia1(uint16_t adresse) {
    uint8_t value;
    switch (adresse) {
        case 0xDC00:  // Adresse: 0xDC00 (CIA 1)
                      // Port A Data Register
                      // Bit 7-0: I/O Pins von Port A
                      //   printf("read port a  ddr %02X\n",cia1.ddra);
                      // printf("Read Stick1 %02x\n",portKeyMap.stick1);
                      // if (portKeyMap.lifeTime>0) {
            // printf("R st1 %02X   %02X\n",portKeyMap.stick1 , cia1.pra);
            if (portKeyMap.stick1) {
                return ~portKeyMap.stick1 & cia1.pra;
            }
            value = cia1.pra;

        case 0xDC01:  // Adresse: 0xDC01 (CIA 1)
                      // Port B Data Register
                      // Bit 7-0: I/O Pins von Port B
            // printf("R st2 %02X\n",portKeyMap.stick2);
            if (portKeyMap.stick2) {
                return ~portKeyMap.stick2;
            }

            cia1.prb = 0xff;

            if (portKeyMap.lifeTime > 0) {
                if (cia1.pra == 0) {
                    cia1.prb &= ~(portKeyMap.portB1 | portKeyMap.portB2);
                    // printf("get 0  val %02x   %02x\n",(uint8_t)~cia1.pra,(uint8_t)~cia1.prb);
                } else {
                    if (~cia1.pra & portKeyMap.portA1) {
                        cia1.prb &= ~(portKeyMap.portB1);
                        // printf("get 1  val %02x   %02x\n",(uint8_t)~cia1.pra,(uint8_t)~cia1.prb);
                    }
                    if (~cia1.pra & portKeyMap.portA2) {
                        cia1.prb &= ~(portKeyMap.portB2);
                        // printf("get 1 Shift val %02x   %02x\n",(uint8_t)~cia1.pra,(uint8_t)~cia1.prb);
                    }
                }
            }
            value = cia1.prb;
            break;

        case 0xDC02:  // Adresse: 0xDC02 (CIA 1)
                      // Data Direction Register A
                      // Bit 7-0: 0 = Input, 1 = Output für Port A
            value = cia1.ddra;
            break;

        case 0xDC03:  // Adresse: 0xDC03 (CIA 1)
                      // Data Direction Register B
                      // Bit 7-0: 0 = Input, 1 = Output für Port B
            value = cia1.ddrb;
            break;

        case 0xDC04:  // Adresse: 0xDC04 (CIA 1)
                      // Timer A, Low Byte
            value = cia1.timerA & 0xff;
            break;

        case 0xDC05:  // Adresse: 0xDC05 (CIA 1)
                      // Timer A, High Byte
                      // 16-bit Timer A (frei programmierbar, Countdown)
            value = (cia1.timerA >> 8) & 0xff;
            break;

        case 0xDC06:  // Adresse: 0xDC06 (CIA 1)
                      // Timer B, Low Byte
            value = cia1.timerB & 0xff;
            break;

        case 0xDC07:  // Adresse: 0xDC07 (CIA 1)
                      // Timer B, High Byte
                      // 16-bit Timer B (frei programmierbar, Countdown)
            value = (cia1.timerB >> 8) & 0xff;
            break;

        case 0xDC08:
        case 0xDC09:
        case 0xDC0A:
        case 0xDC0B:
            value = read_regRTC(&cia1RTC, adresse);
            break;

        case 0xDC0C:  // Adresse: 0xDC0C (CIA 1)
                      // Serial Data Register (SDR)
                      // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation
            value = cia1.sdr;
            break;

        case 0xDC0D:  // Adresse: 0xDC0D (CIA 1)
                      // Interrupt Control Register (ICR)
                      // Bit 7: Setzt Interrupt-Flag
                      // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                      // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                      // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                      // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das
                      // serielle Schieberegister. Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke
                      // am FLAG-Pin.
            value = cia1.icr;
            // printf("read CAI 1 ICR %02x\n",value);
            cia1.icr = 0;
            doIRQ &= ~(CIA1_A_IRQ | CIA1_B_IRQ);
            break;

        case 0xDC0E:  // Adresse: 0xDC0E (CIA 1)
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
            value = cia1.cra;
            break;

        case 0xDC0F:  // Adresse: 0xDC0F (CIA 1)
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
            value = cia1.crb;
            break;

        default:
            // Ungültige Adresse
            break;
    }

    return value;
}

void updateCia1(uint8_t clkCount) {
    static int32_t sClkCount = 0;

    sClkCount = sClkCount + clkCount;
    if (sClkCount >= TENS_OF_SEC) {
        sClkCount = sClkCount - TENS_OF_SEC;
        increment_rtc(&cia1RTC);
    }

    if (ciaTimer_ctrl) {
        if (ciaTimer_ctrl & (TIMERA1_RUN)) {
            cia1.timerA -= clkCount;
            if (cia1.timerA <= 0) {
                if (cia1.icrMask & 0x01) {
                    doIRQ |= CIA1_A_IRQ;
                    cia1.icr |= 0x81;
                }
                if (ciaTimer_ctrl & TIMERA1_RELOAD) {
                    cia1.timerA = cia1.reloadTimerA;
                } else {
                    cia1.timerA = 0;
                    ciaTimer_ctrl &= ~TIMERA1_RUN;
                }
            }
        }
        if (ciaTimer_ctrl & (TIMERB1_RUN)) {
            cia1.timerB -= clkCount;
            if (cia1.timerB <= 0) {
                if (cia1.icrMask & 0x02) {
                    doIRQ |= CIA1_B_IRQ;
                    cia1.icr |= 0x82;
                }
                if (ciaTimer_ctrl & TIMERB1_RELOAD) {
                    cia1.timerB = cia1.reloadTimerB;
                } else {
                    cia1.timerB = 0;
                    ciaTimer_ctrl &= ~TIMERB1_RUN;
                }
            }
        }
    }
    if (portKeyMap.lifeTime > 0) {
        portKeyMap.lifeTime -= clkCount;
    }
}

void writeCia1toTxtFile(void) { writeCiatoFile("cia1_register_info.txt", &cia1, &cia1RTC); }
