#include <stdint.h>
#include <stdio.h>

#include "cia.h"
#include "ciaRtc.h"


RTC_CIA cia1RTC = {.baseAddress = CIA1_BASE}; // CIA 1
RTC_CIA cia2RTC = {.baseAddress = CIA2_BASE}; // CIA 2


// Hilfsfunktionen zur Umrechnung zwischen BCD und Binär
static uint8_t bcd_to_bin(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd >> 4) * 10);
}

static uint8_t bin_to_bcd(uint8_t bin) {
    return ((bin / 10) << 4) | (bin % 10);
}

// Funktion zum Inkrementieren der RTC-Zeiten
void increment_rtc(RTC_CIA *cia) {
    // Inkrementiere todTenth
    uint8_t tenths = cia->TOD_10TH;
    tenths++;
    if (tenths > 9) {  // Zehntelsekunden erreichen den Maximalwert 9
        tenths = 0;
        cia->TOD_10TH = tenths;

        // Erhöhe todSec um 1
        uint8_t seconds = bcd_to_bin(cia->TOD_SEC);
        seconds++;
        if (seconds >= 60) {  // Wenn Sekunden 60 erreichen, setze auf 0 und erhöhe Minuten
            seconds = 0;
            cia->TOD_SEC = bin_to_bcd(seconds);

            // Erhöhe todMin um 1
            uint8_t minutes = bcd_to_bin(cia->TOD_MIN);
            minutes++;
            if (minutes >= 60) {  // Wenn Minuten 60 erreichen, setze auf 0 und erhöhe Stunden
                minutes = 0;
                cia->TOD_MIN = bin_to_bcd(minutes);

                // Erhöhe todHr um 1
                uint8_t hours = bcd_to_bin(cia->TOD_HR);
                hours++;
                if (hours >= 24) {  // Stunden zurücksetzen, wenn 24 erreicht (24-Stunden-Format)
                    hours = 0;
                }
                cia->TOD_HR = bin_to_bcd(hours);
            } else {
                cia->TOD_MIN = bin_to_bcd(minutes);
            }
        } else {
            cia->TOD_SEC= bin_to_bcd(seconds);
        }
    } else {
        cia->TOD_10TH = tenths;
    }
    // Alarmprüfung nach jedem Zeitschritt
    if (cia->CRB7 == 1 && // Nur prüfen, wenn im RTC-Modus
        cia->TOD_HR == cia->ALARM_HR &&
        cia->TOD_MIN == cia->ALARM_MIN &&
        cia->TOD_SEC == cia->ALARM_SEC &&
        cia->TOD_10TH == cia->ALARM_10TH) {
        cia->alarmTriggered = 1;
        printf("Alarm ausgelöst!\n");
    }
}

// Funktion zum Schreiben in ein Register (RTC oder Alarm, abhängig von CRB7)
void write_regRTC(RTC_CIA *cia, uint16_t regAddr, uint8_t value) {
    // printf("Write to RTC %04x  val = %02x\n",regAddr,value);
    switch (regAddr&0xff) {
        case 0x0B:  // TOD_HR
            if (cia->CRB7) {
                cia->ALARM_HR = value;
            } else {
                cia->TOD_HR = value;
            }
            break;
        case 0x0A:  // TOD_MIN
            if (cia->CRB7) {
                cia->ALARM_MIN = value;
            } else {
                cia->TOD_MIN = value;
            }
            break;
        case 0x09:  // TOD_SEC
            if (cia->CRB7) {
                cia->ALARM_SEC = value;
            } else {
                cia->TOD_SEC = value;
            }
            break;
        case 0x08:  // TOD_10TH
            cia->CRB7 = value & 0x80;
            if (cia->CRB7) {
                cia->ALARM_10TH = value & 0x0f;
            } else {
                cia->TOD_10TH = value & 0x0f;
            }
            break;
    }
}

// Funktion zum Lesen eines Registers (RTC oder Alarm, abhängig von CRB7)
uint8_t read_regRTC(RTC_CIA *cia, uint16_t regAddr) {
uint8_t value=0;
    if (cia->latchActive) { // Bei aktivem Latch-Status aus LATCH_* lesen
        switch (regAddr & 0xff) {
            case 0x0B: value = cia->LATCH_HR; break;
            case 0x0A: value = cia->LATCH_MIN; break;
            case 0x09: value = cia->LATCH_SEC; break;
            case 0x08: value = cia->LATCH_10TH;break;
        }
    } else { // Ohne Latch, direktes Lesen aus TOD_*
        switch (regAddr & 0xff) {
            case 0x0B: 
                cia->latchActive = 1;
                cia->LATCH_HR = cia->TOD_HR;
                cia->LATCH_MIN = cia->TOD_MIN;
                cia->LATCH_SEC = cia->TOD_SEC;
                cia->LATCH_10TH = cia->TOD_10TH;
                value = cia->TOD_HR; break;
            case 0x0A: 
                value = cia->TOD_MIN; break;
            case 0x09: 
                value = cia->TOD_SEC; break;
            case 0x08: 
                cia->latchActive = 0; // Latch deaktivieren durch Schreiben auf TOD_10TH
                value =  cia->TOD_10TH; break;
        }
    }
    // printf("read from RTC %04x  val = %02x\n",regAddr,value);
    return value;
}
