#ifndef SID_H
#define SID_H

#include <stdint.h>

typedef struct {
    uint8_t freq_voice1_low;         // $D400 / 54272: Frequenz Stimme 1 (Low-Byte)
    uint8_t freq_voice1_high;        // $D401 / 54273: Frequenz Stimme 1 (High-Byte)
    uint8_t duty_cycle_voice1_low;   // $D402 / 54274: Tastverhältnis Stimme 1 für Rechteck (Low-Byte)
    uint8_t duty_cycle_voice1_high;  // $D403 / 54275: Tastverhältnis Stimme 1 für Rechteck (High-Byte 0...15)
    uint8_t waveform_voice1;  // $D404 / 54276: Wellenform Stimme 1 (Rauschen 129; Rechteck 65; Sägezahn 33; Dreieck 17)
                              //              Bit 0: Hüllkurvengenerator-Start, Bit 1: Sync, Bit 2: Ring, Bit 3: Test
                              //              Bit 4-7: Dreieck/Sägezahn/Rechteck/Rauschen
    uint8_t attack_decay_voice1;     // $D405 / 54277: Anschlag (0*16 hart ... 15*16 weich) / Abschwellen Stimme 1 (+ 0
                                     // hart ... 15 weich)
    uint8_t sustain_release_voice1;  // $D406 / 54278: Halten (0*16 stumm ... 15*16 laut) / Ausklingen Stimme 1 (+ 0
                                     // schnell ... 15 langsam)

    uint8_t freq_voice2_low;         // $D407 / 54279: Frequenz Stimme 2 (Low-Byte)
    uint8_t freq_voice2_high;        // $D408 / 54280: Frequenz Stimme 2 (High-Byte)
    uint8_t duty_cycle_voice2_low;   // $D409 / 54281: Tastverhältnis Stimme 2 für Rechteck (Low-Byte)
    uint8_t duty_cycle_voice2_high;  // $D40A / 54282: Tastverhältnis Stimme 2 für Rechteck (High-Byte 0...15)
    uint8_t waveform_voice2;  // $D40B / 54283: Wellenform Stimme 2 (Rauschen 129; Rechteck 65; Sägezahn 33; Dreieck 17)
    uint8_t attack_decay_voice2;     // $D40C / 54284: Anschlag (0*16 hart ... 15*16 weich) / Abschwellen Stimme 2 (+ 0
                                     // hart ... 15 weich)
    uint8_t sustain_release_voice2;  // $D40D / 54285: Halten (0*16 stumm ... 15*16 laut) / Ausklingen Stimme 2 (+ 0
                                     // schnell ... 15 langsam)

    uint8_t freq_voice3_low;         // $D40E / 54286: Frequenz Stimme 3 (Low-Byte)
    uint8_t freq_voice3_high;        // $D40F / 54287: Frequenz Stimme 3 (High-Byte)
    uint8_t duty_cycle_voice3_low;   // $D410 / 54288: Tastverhältnis Stimme 3 für Rechteck (Low-Byte)
    uint8_t duty_cycle_voice3_high;  // $D411 / 54289: Tastverhältnis Stimme 3 für Rechteck (High-Byte 0...15)
    uint8_t waveform_voice3;  // $D412 / 54290: Wellenform Stimme 3 (Rauschen 129; Rechteck 65; Sägezahn 33; Dreieck 17)
    uint8_t attack_decay_voice3;     // $D413 / 54291: Anschlag (0*16 hart ... 15*16 weich) / Abschwellen Stimme 3 (+ 0
                                     // hart ... 15 weich)
    uint8_t sustain_release_voice3;  // $D414 / 54292: Halten (0*16 stumm ... 15*16 laut) / Ausklingen Stimme 3 (+ 0
                                     // schnell ... 15 langsam)

    uint8_t filter_cutoff_low;   // $D415 / 54293: Grenzfrequenzfilter (Low-Byte 0...7)
    uint8_t filter_cutoff_high;  // $D416 / 54294: Grenzfrequenzfilter (High-Byte)
    uint8_t filter_resonance;    // $D417 / 54295: Resonanz; 0 keine; 15*16 stark; (+ Filter für 1. Stimme 1; 2. Stimme
                                 // 2; 3. Stimme 4; extern 8)
    uint8_t volume_and_filter;   // $D418 / 54296: Lautstärke für alle Stimmen; 0 stumm; 15 ganz laut; (+ Filtermodus)

    uint8_t paddle_x;  // $D419 / 54297: Paddle X-Wert
    uint8_t paddle_y;  // $D41A / 54298: Paddle Y-Wert
    uint8_t osc3;      // $D41B / 54299: Oszillator Stimme 3
    uint8_t env3;      // $D41C / 54300: Hüllkurve Stimme 3
    uint8_t unused1;   // $D41D / 54301: unbenutzt
    uint8_t unused2;   // $D41E / 54302: unbenutzt
    uint8_t unused3;   // $D41F / 54303: unbenutzt
} sid_t;

#define SID_START_ADDR 0xD400
#define SID_END_ADDR 0xD7ff

extern sid_t sidRegister;

void sidWrite(uint16_t addr, uint8_t value);
uint8_t sidRead(uint16_t addr);

#endif