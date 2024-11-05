#ifndef CAI_RTC_H
#define CAI_RTC_H

#define CIA1_BASE 0xDC00
#define CIA2_BASE 0xDD00

#if 0
    uint8_t todTenth;    // Time of Day Clock (TOD), Zehntelsekunden
                          // Adresse: 0xDC08 (CIA 1), 0xDD08 (CIA 2)
                          // Bit 7-0: Zehntelsekunden

    uint8_t todSec;      // Time of Day Clock (TOD), Sekunden
                          // Adresse: 0xDC09 (CIA 1), 0xDD09 (CIA 2)
                          // Bit 7-0: Sekunden (00-59)

    uint8_t todMin;      // Time of Day Clock (TOD), Minuten
                          // Adresse: 0xDC0A (CIA 1), 0xDD0A (CIA 2)
                          // Bit 7-0: Minuten (00-59)

    uint8_t todHr;       // Time of Day Clock (TOD), Stunden
                          // Adresse: 0xDC0B (CIA 1), 0xDD0B (CIA 2)
                          // Bit 7-0: Stunden (00-23 im 24-Stunden-Format)

#endif

// Struktur für die RTC- und Alarmregister der CIA
typedef struct {
    uint8_t TOD_HR;
    uint8_t TOD_MIN;
    uint8_t TOD_SEC;
    uint8_t TOD_10TH;
    uint8_t ALARM_HR;
    uint8_t ALARM_MIN;
    uint8_t ALARM_SEC;
    uint8_t ALARM_10TH;
    uint8_t CRB7;            // Kontrollregister zur Auswahl von RTC/Alarm-Modus
    uint8_t alarmTriggered;  // Flag für ausgelösten Alarm
    uint8_t latchActive;     // Flag für Latch-Status
    uint8_t LATCH_HR;
    uint8_t LATCH_MIN;
    uint8_t LATCH_SEC;
    uint8_t LATCH_10TH;
    uint16_t baseAddress;  // Basisadresse für CIA (CIA1 oder CIA2)
} RTC_CIA;

extern RTC_CIA cia1RTC;
extern RTC_CIA cia2RTC;

void increment_rtc(RTC_CIA* cia);
void write_regRTC(RTC_CIA* cia, uint16_t regAddr, uint8_t value);
uint8_t read_regRTC(RTC_CIA* cia, uint16_t regAddr);
#endif