#ifndef CIA_H
#define CIA_H

#include <stdint.h>

#include "ciaRtc.h"

#define TIMERA1_RUN     0x0001
#define TIMERB1_RUN     0x0002
#define TIMERA2_RUN     0x0004
#define TIMERB2_RUN     0x0008
#define TIMERA1_RELOAD  0x0010
#define TIMERB1_RELOAD  0x0020
#define TIMERA2_RELOAD  0x0040
#define TIMERB2_RELOAD  0x0080

#define TENS_OF_SEC  100000


#define CIA1_A_IRQ      0x0010
#define CIA1_B_IRQ      0x0020

#define CIA2_A_NIM      0x0100
#define CIA2_B_NIM      0x0200

// CIA 1 und CIA 2 Registermap (Memory Mapping)
// CIA 1 ist unter 0xDC00 und CIA 2 unter 0xDD00 im Speicher zu finden

typedef struct {
    uint8_t pra;          // Port A Data Register
                          // Adresse: 0xDC00 (CIA 1), 0xDD00 (CIA 2)
                          // Bit 7-0: I/O Pins von Port A

    uint8_t prb;          // Port B Data Register
                          // Adresse: 0xDC01 (CIA 1), 0xDD01 (CIA 2)
                          // Bit 7-0: I/O Pins von Port B

    uint8_t ddra;         // Data Direction Register A
                          // Adresse: 0xDC02 (CIA 1), 0xDD02 (CIA 2)
                          // Bit 7-0: 0 = Input, 1 = Output für Port A

    uint8_t ddrb;         // Data Direction Register B
                          // Adresse: 0xDC03 (CIA 1), 0xDD03 (CIA 2)
                          // Bit 7-0: 0 = Input, 1 = Output für Port B

    int32_t timerA;    // Timer A, Low Byte
//    uint8_t timerA_lo;    // Timer A, Low Byte
                          // Adresse: 0xDC04 (CIA 1), 0xDD04 (CIA 2)

//    uint8_t timerA_hi;    // Timer A, High Byte
                          // Adresse: 0xDC05 (CIA 1), 0xDD05 (CIA 2)
                          // 16-bit Timer A (frei programmierbar, Countdown)

    int32_t timerB;    // Timer B, Low Byte
//    uint8_t timerB_lo;    // Timer B, Low Byte
                          // Adresse: 0xDC06 (CIA 1), 0xDD06 (CIA 2)

//    uint8_t timerB_hi;    // Timer B, High Byte
                          // Adresse: 0xDC07 (CIA 1), 0xDD07 (CIA 2)
                          // 16-bit Timer B (frei programmierbar, Countdown)

    uint8_t sdr;          // Serial Data Register (SDR)
                          // Adresse: 0xDC0C (CIA 1), 0xDD0C (CIA 2)
                          // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation

    uint8_t icr;          // Interrupt Control Register (ICR)
                          // Adresse: 0xDC0D (CIA 1), 0xDD0D (CIA 2)
                          // Bit 7: Setzt Interrupt-Flag
                          // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                          // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                          // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                          // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das serielle Schieberegister.
                          // Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke am FLAG-Pin.
    uint8_t cra;          // Control Register A (CRA)
                          // Adresse: 0xDC0E (CIA 1), 0xDD0E (CIA 2)
                          // Bit 0: 0 = Stop Timer; 1 = Start Timer
                          // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 6 an
                          // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 6 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 6 für einen Systemtaktzyklus High
                          // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                          // Bit 4: 1 = Einmalig Latch in den Timer laden
                          // Bit 5: 0 = Timer wird mit der Systemfrequenz getaktet, 1 = Timer wird von positiven Flanken am CNT-Pin getaktet
                          // Bit 6: Richtung des seriellen Schieberegisters, 0 = SP-Pin ist Eingang (lesen), 1 = SP-Pin ist Ausgang (schreiben)
                          // Bit 7: Echtzeituhr, 0 = 60 Hz, 1 = 50 Hz an Pin 19



    uint8_t crb;          // Control Register B (CRB)
                          // Adresse: 0xDC0F (CIA 1), 0xDD0F (CIA 2)
                          // Bit 0: 0 = Stop Timer; 1 = Start Timer
                          // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 7 an
                          // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 7 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 7 für einen Systemtaktzyklus High
                          // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                          // Bit 4: 1 = Einmalig Latch in den Timer laden
                          // Bit 5..6: Timer wird getaktet ...
                                // %00 = mit der Systemfrequenz
                                // %01 = von positiver Flanke am CNT-Pin
                                // %10 = vom Unterlauf des Timer A
                                // %11 = vom Unterlauf des Timer A, wenn CNT-Pin High ist
                          // Bit 7: 0 = Schreiben in die TOD-Register setzt die Uhrzeit, 1 = Schreiben in die TOD-Register setzt die Alarmzeit                          

/***********************************************************************************************************/                          
// Help Register for emulation                          

    uint8_t icrMask; 
                          // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                          // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                          // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                          // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das serielle Schieberegister.
                          // Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke am FLAG-Pin.

    uint16_t reloadTimerA;                          
    uint16_t reloadTimerB;                 
} CIA;

extern const uint8_t portNrToBitMap[8];
/*
const uint8_t portNrToNBitMap[8] = {
    0xFE,  // 11111110 -> Bit 0 gesetzt, Position 1
    0xFD,  // 11111101 -> Bit 1 gesetzt, Position 2
    0xFB,  // 11111011 -> Bit 2 gesetzt, Position 3
    0xF7,  // 11110111 -> Bit 3 gesetzt, Position 4
    0xEF,  // 11101111 -> Bit 4 gesetzt, Position 5
    0xDF,  // 11011111 -> Bit 5 gesetzt, Position 6
    0xBF,  // 10111111 -> Bit 6 gesetzt, Position 7
    0x7F   // 01111111 -> Bit 7 gesetzt, Position 8
};

*/
typedef struct {
    int32_t lifeTime;
    uint8_t portA1;
    uint8_t portB1;
    uint8_t portA2;
    uint8_t portB2;
    uint8_t stick1;
    uint8_t stick2;
} portKeyMap_t;

extern portKeyMap_t portKeyMap;


#define CIA1ADDR   0xDC00  // Adresse für CIA 1
#define CIA1END    0xDC0F
#define CIA2ADDR   0xDD00  // Adresse für CIA 2
#define CIA2END    0xDD0F 

extern uint8_t ciaGetvidoebank(void);
extern void writeCia1(uint16_t addr,uint8_t value); 
extern void writeCia2(uint16_t addr,uint8_t value); 
extern uint8_t readCia1(uint16_t addr); 
extern uint8_t readCia2(uint16_t addr); 
extern void updateCia1(uint8_t clkCount);
extern void updateCia2(uint8_t clkCount); 
extern void writeCia1toTxtFile(void);
extern void writeCia2toTxtFile(void);
extern int writeCiatoFile(const char *,CIA * cia,RTC_CIA * ciaRTC);

#endif