
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdio.h>

#include "sid.h"


#define SAMPLE_RATE 22000             // Abtastrate
#define AMPLITUDE 700 // (32000/3 / 15)           // Amplitude des Signals
#define BUFFER_CHUNK_SIZE  400         // Größe eines Puffersegments
#define BUFFER_CHUNK_COUNT 4          // Anzahl der Puffersegmente (für Ringpuffer)
#define MAX_VOICE 3


#pragma comment(lib, "winmm.lib")

HWAVEOUT hWaveOut;
WAVEFORMATEX wfx;
WAVEHDR headers[BUFFER_CHUNK_COUNT];
int16_t buffers[BUFFER_CHUNK_COUNT][BUFFER_CHUNK_SIZE];
int currentChunk = 0;                 // Aktuelles Segment im Ringpuffer

#define DBGLEN 50000

int16_t dbg[DBGLEN];

enum soundType {    
PWM,
SAW,
TRI,
NOISE,
NONE
} ;


enum soundState {    
MUTE,
ATTACK,
DECAY,
RELEASE,
SUSTAIN
} ;

typedef struct {
double time;
float freq;
float dutyCycle;
uint16_t ring;
uint16_t sync;
float attack; // in s
float decay;  // in s
uint16_t sustainVol; 
float release; // in s
uint16_t keyBit;
enum soundType type;
enum soundState state; 
} voice_t;

float attack[] = {0.002, 0.008, 0.016, 0.024, 0.038, 0.056, 0.068, 0.080, 0.100, 0.250, 0.500, 0.800, 1.0, 3.0, 5.0, 8.0};

// Array für Decay-Zeiten in Millisekunden
float decay[] = {0.006, 0.024, 0.048, 0.072, 0.114, 0.168, 0.204, 0.240, 0.300, 0.750, 1.5, 2.4, 3.0, 9.0, 15.0, 24.0};

// Array für Release-Zeiten in Millisekunden
float release[] = {6, 24, 48, 72, 114, 168, 204, 240, 300, 750, 1500, 2400, 3000, 9000, 15000, 24000};


voice_t voice[MAX_VOICE];


sid_t sidRegister;

uint8_t sidRead(uint16_t addr) {
    uint8_t* ptr;

    ptr = (uint8_t*)&sidRegister + (addr - SID_START_ADDR);
    return *ptr;
}

void sidWrite(uint16_t addr, uint8_t value) {
     // return;
  // Switch-Anweisung für addr
    switch (addr) {
        case 0xD400:  // Frequenz Stimme 1 (Low-Byte)
            sidRegister.freq_voice1_low = value;
            voice[0].freq = 440.0 * (float)((sidRegister.freq_voice1_high << 8) | sidRegister.freq_voice1_low) / 7493.0;
            break;
        case 0xD401:  // Frequenz Stimme 1 (High-Byte)
            sidRegister.freq_voice1_high = value;
            voice[0].freq = 440.0 * (float)((sidRegister.freq_voice1_high << 8) | sidRegister.freq_voice1_low) / 7493.0;
            break;
        case 0xD402:  // Tastverhältnis Stimme 1 (Low-Byte)
            sidRegister.duty_cycle_voice1_low = value;
            voice[0].dutyCycle =  (float)(((sidRegister.duty_cycle_voice1_high << 8) & 0x0f)  | sidRegister.duty_cycle_voice1_low) / 4096.0;
            break;
        case 0xD403:  // Tastverhältnis Stimme 1 (High-Byte)
            sidRegister.duty_cycle_voice1_high = value;
            voice[0].dutyCycle =  (float)(((sidRegister.duty_cycle_voice1_high << 8) & 0x0f)  | sidRegister.duty_cycle_voice1_low) / 4096.0;
            break;
        case 0xD404:  // Wellenform Stimme 1
            sidRegister.waveform_voice1 = value;
            if (sidRegister.waveform_voice1 & 0x10) {
                voice[0].type=TRI;
            } else if (sidRegister.waveform_voice1 & 0x20) {
                voice[0].type=SAW;
            } else if (sidRegister.waveform_voice1 & 0x40) {
                voice[0].type=PWM;
            } else if (sidRegister.waveform_voice1 & 0x80) {
                voice[0].type=NOISE;
            }                            
            voice[0].ring = sidRegister.waveform_voice1 & 0x04;
            voice[0].sync = sidRegister.waveform_voice1 & 0x02;
            
            if (sidRegister.waveform_voice1 & 0x01) {
                if (voice[0].keyBit==0) {
                    voice[0].state = ATTACK;
                    voice[0].keyBit = 1;
                    voice[0].time = 0;
                }
            }  else {
                if (voice[0].keyBit) {
                    voice[0].keyBit = 0;
                    voice[0].time = 0;
                    voice[0].state  = RELEASE;
                }
            }

            break;
        case 0xD405:  // Attack/Decay Stimme 1
            sidRegister.attack_decay_voice1 = value;
            voice[0].attack = attack[(sidRegister.attack_decay_voice1 >> 4) & 0x0f];
            voice[0].decay  = decay[sidRegister.attack_decay_voice1 & 0x0f];
            break;
        case 0xD406:  // Sustain/Release Stimme 1
            sidRegister.sustain_release_voice1 = value;
            voice[0].sustainVol = (sidRegister.sustain_release_voice1 >> 4) & 0x0f;
            voice[0].release   = release[sidRegister.sustain_release_voice1 & 0x0f];
            break;

        case 0xD407:  // Frequenz Stimme 2 (Low-Byte)
            sidRegister.freq_voice2_low = value;
            voice[1].freq = 440.0 * (float)((sidRegister.freq_voice2_high << 8) | sidRegister.freq_voice2_low) / 7493.0;
            break;
        case 0xD408:  // Frequenz Stimme 2 (High-Byte)
            sidRegister.freq_voice2_high = value;
            voice[1].freq = 440.0 * (float)((sidRegister.freq_voice2_high << 8) | sidRegister.freq_voice2_low) / 7493.0;
            break;
        case 0xD409:  // Tastverhältnis Stimme 2 (Low-Byte)
            sidRegister.duty_cycle_voice2_low = value;
            voice[1].dutyCycle =  (float)(((sidRegister.duty_cycle_voice2_high << 8) & 0x0f)  | sidRegister.duty_cycle_voice2_low) / 4096.0;
            break;
        case 0xD40A:  // Tastverhältnis Stimme 2 (High-Byte)
            sidRegister.duty_cycle_voice2_high = value;
            voice[1].dutyCycle =  (float)(((sidRegister.duty_cycle_voice2_high << 8) & 0x0f)  | sidRegister.duty_cycle_voice2_low) / 4096.0;
            break;
        case 0xD40B:  // Wellenform Stimme 2
            sidRegister.waveform_voice2 = value;
            if (sidRegister.waveform_voice2 & 0x10) {
                voice[1].type=TRI;
            } else if (sidRegister.waveform_voice2 & 0x20) {
                voice[1].type=SAW;
            } else if (sidRegister.waveform_voice2 & 0x40) {
                voice[1].type=PWM;
            } else if (sidRegister.waveform_voice2 & 0x80) {
                voice[1].type=NOISE;
            }                            
            voice[1].ring = sidRegister.waveform_voice2 & 0x04;
            voice[1].sync = sidRegister.waveform_voice2 & 0x02;
            
            if (sidRegister.waveform_voice2 & 0x01) {
                if (voice[1].keyBit==0) {
                    voice[1].state = ATTACK;
                    voice[1].keyBit = 1;
                    voice[1].time = 0;
                }
            }  else {
                if (voice[1].keyBit) {
                    voice[1].keyBit = 0;
                    voice[1].time = 0;
                    voice[1].state  = RELEASE;
                }
            }
            break;
        case 0xD40C:  // Attack/Decay Stimme 2
            sidRegister.attack_decay_voice2 = value;
            voice[1].attack = attack[(sidRegister.attack_decay_voice2>>4)&0x0f];
            voice[1].decay  = decay[sidRegister.attack_decay_voice2&0x0f];
            break;
        case 0xD40D:  // Sustain/Release Stimme 2
            sidRegister.sustain_release_voice2 = value;
            voice[1].sustainVol = (sidRegister.sustain_release_voice2>>4)&0x0f;
            voice[1].release   = release[sidRegister.sustain_release_voice2&0x0f];
            break;

        case 0xD40E:  // Frequenz Stimme 3 (Low-Byte)
            sidRegister.freq_voice3_low = value;
            voice[2].freq = 440.0 * (float)((sidRegister.freq_voice3_high << 8) | sidRegister.freq_voice3_low) / 7493.0;
            break;
        case 0xD40F:  // Frequenz Stimme 3 (High-Byte)
            sidRegister.freq_voice3_high = value;
            voice[2].freq = 440.0 * (float)((sidRegister.freq_voice3_high << 8) | sidRegister.freq_voice3_low) / 7493.0;
            break;
        case 0xD410:  // Tastverhältnis Stimme 3 (Low-Byte)
            sidRegister.duty_cycle_voice3_low = value;
            voice[2].dutyCycle =  (float)(((sidRegister.duty_cycle_voice3_high << 8) & 0x0f)  | sidRegister.duty_cycle_voice3_low) / 4096.0;
            break;
        case 0xD411:  // Tastverhältnis Stimme 3 (High-Byte)
            sidRegister.duty_cycle_voice3_high = value;
            voice[2].dutyCycle =  (float)(((sidRegister.duty_cycle_voice3_high << 8) & 0x0f)  | sidRegister.duty_cycle_voice3_low) / 4096.0;
            break;
        case 0xD412:  // Wellenform Stimme 3
            sidRegister.waveform_voice3 = value;
            if (sidRegister.waveform_voice3 & 0x10) {
                voice[2].type=TRI;
            } else if (sidRegister.waveform_voice3 & 0x20) {
                voice[2].type=SAW;
            } else if (sidRegister.waveform_voice3 & 0x40) {
                voice[2].type=PWM;
            } else if (sidRegister.waveform_voice3 & 0x80) {
                voice[2].type=NOISE;
            }                            
            voice[2].ring = sidRegister.waveform_voice3 & 0x04;
            voice[2].sync = sidRegister.waveform_voice3 & 0x02;
            
            if (sidRegister.waveform_voice3 & 0x01) {
                if (voice[2].keyBit==0) {
                    voice[2].state = ATTACK;
                    voice[2].keyBit = 1;
                    voice[2].time = 0;
                }
            }  else {
                if (voice[2].keyBit) {
                    voice[2].keyBit = 0;
                    voice[2].time = 0;
                    voice[2].state  = RELEASE;
                }
            }
            break;
        case 0xD413:  // Attack/Decay Stimme 3
            sidRegister.attack_decay_voice3 = value;
            voice[2].attack = attack[(sidRegister.attack_decay_voice3>>4)&0x0f];
            voice[2].decay  = decay[sidRegister.attack_decay_voice3&0x0f];
            break;
        case 0xD414:  // Sustain/Release Stimme 3
            sidRegister.sustain_release_voice3 = value;
            voice[2].sustainVol = (sidRegister.sustain_release_voice3>>4) & 0x0f;
            voice[2].release   = release[sidRegister.sustain_release_voice3 & 0x0f];
            break;

        case 0xD415:  // Filter Cutoff (Low-Byte)
            sidRegister.filter_cutoff_low = value;
            break;
        case 0xD416:  // Filter Cutoff (High-Byte)
            sidRegister.filter_cutoff_high = value;
            break;
        case 0xD417:  // Filter Resonance
            sidRegister.filter_resonance = value;
            break;
        case 0xD418:  // Lautstärke und Filtermodi
        // printf("Set volume end filter to %02x\n",value);
            sidRegister.volume_and_filter = value;
            break;

        case 0xD419:  // Paddle X-Wert
            // sidRegister.paddle_x = value;  // read only
            break;
        case 0xD41A:  // Paddle Y-Wert
            // sidRegister.paddle_y = value;  // read only
            break;
        case 0xD41B:  // Oszillator 3
            // sidRegister.osc3 = value; // read only
            break;
        case 0xD41C:  // Hüllkurve 3
            // sidRegister.env3 = value;  // read only
            break;

        default:
            // Kein gültiger addr
            break;
    }
}


float mysin(int32_t pos, float freq) {

    if (freq>0) {     
        float phase =  ((int32_t)(pos * freq) % SAMPLE_RATE  / (float) SAMPLE_RATE); 
        return sin(phase*2*3.14159265358979323846);
    } 

    return 0;        
}


float tri(int32_t pos, float freq) {

    if (freq>0) {     
        float phase =  ((int32_t)(pos * freq) % SAMPLE_RATE  / (float) SAMPLE_RATE); 
        if (phase<0.25) {
            return phase * 4.0;
        }
        if (phase<0.75) {
            return (0.5 - phase) * 4;
        }
        return (phase-1.0) * 4;
    } 

    return 0;        
}

float saw(int32_t pos, float freq) {

    if (freq>0) {     
        float phase =  ((int32_t)(pos * freq) % SAMPLE_RATE  / (float) SAMPLE_RATE); 
        return (phase * 2.0 - 1.0) ;
    } 

    return 0;        
}

float noise(int32_t pos, float freq) {

    return  (float) (rand() % 10000 ) / 10000.0;        
}



float pwm(int32_t pos, float freq,float dutyCycle) {

    if (freq>0) {     
        float phase =  ((int32_t)(pos * freq) % SAMPLE_RATE  / (float) SAMPLE_RATE); 
        if (phase > dutyCycle) {
            return 1;
        } else {
            return -1;
        }
    } 

    return 0;        
}

// Füllt ein Segment des Ringpuffers mit Sinuswellen-Daten
void fill_buffer_with_tone(int chunkIndex) {
     int max=0;
     int min=0;
    static int32_t timeInPeriode[MAX_VOICE]={0};
    static int cnt=0;
    int16_t helpBuffer[MAX_VOICE][BUFFER_CHUNK_SIZE];
    float volume;

    for (int voiceCNT=0;voiceCNT<MAX_VOICE;voiceCNT++) {
        float envelop =0;        


        for (int i = 0; i < BUFFER_CHUNK_SIZE; i++) {

            volume = voice[voiceCNT].sustainVol * AMPLITUDE; 

            if (voice[voiceCNT].keyBit) {
                if (voice[voiceCNT].state==ATTACK)  {
                    volume = (sidRegister.volume_and_filter & 0x0f) * AMPLITUDE; 
                    if  (voice[voiceCNT].time < voice[voiceCNT].attack) {
                        envelop = voice[voiceCNT].time / voice[voiceCNT].attack;
                        // printf("time %.6f env %.3f  attack %.3f  vol %.1f\n",voice[voiceCNT].time,envelop,voice[voiceCNT].attack,volume);
                    } else {
                        voice[voiceCNT].state=DECAY;
                        voice[voiceCNT].time = 0;
                    }
                } 
                                
                if (voice[voiceCNT].state==DECAY)  {
                    if  (voice[voiceCNT].time < voice[voiceCNT].decay) {
                        float startVol = (sidRegister.volume_and_filter & 0x0f) * AMPLITUDE; 
                        float offset = volume / startVol;            
                        float scale = (1.0 - offset) * (voice[voiceCNT].decay - voice[voiceCNT].time) / voice[voiceCNT].decay;
                        volume = startVol;
                        envelop = scale + offset;
                        // printf("time %.6f env %.3f  decay %.3f  vol %.1f\n",voice[voiceCNT].time,envelop,voice[voiceCNT].attack,volume);
                    } else {
                        voice[voiceCNT].state=SUSTAIN;
                        voice[voiceCNT].time = 0;
                    }
                }                       
                if (voice[voiceCNT].state==SUSTAIN)  {                
                    envelop=1;
                }
            } else {
                voice[voiceCNT].state=RELEASE;
                if  (voice[voiceCNT].time < voice[voiceCNT].release) {
                    envelop = (voice[voiceCNT].release - voice[voiceCNT].time) / voice[voiceCNT].release;
                } else {
                    envelop = 0;
                    voice[voiceCNT].state = MUTE;
                }
            }


            switch (voice[voiceCNT].type) {
            case TRI:             
                    helpBuffer[voiceCNT][i] = (int16_t)(volume * envelop * tri(timeInPeriode[voiceCNT],voice[voiceCNT].freq));
                break;
            case SAW:             
                    helpBuffer[voiceCNT][i] = (int16_t)(volume * envelop * saw(timeInPeriode[voiceCNT],voice[voiceCNT].freq));
                break;
            case PWM:             
                    helpBuffer[voiceCNT][i] = (int16_t)(volume * envelop * pwm(timeInPeriode[voiceCNT],voice[voiceCNT].freq,voice[voiceCNT].dutyCycle));
                break;
            case NOISE:             
                    helpBuffer[voiceCNT][i] = (int16_t)(volume * envelop * noise(timeInPeriode[voiceCNT],voice[voiceCNT].freq));
                break;
            
            default:
                helpBuffer[voiceCNT][i] = 0;
                break;
            }

#ifdef DEBUG            
            dbg[cnt] = helpBuffer[voiceCNT][i];
            cnt++;
            if (cnt==DBGLEN) {
                FILE* filePointer = fopen("wave.txt", "w");

                for (int i=0;i<DBGLEN;i++) {
                    fprintf(filePointer,"%d\n",dbg[i]);
                }
                fclose(filePointer);
                exit(1);
            }
#endif
            
            //  printf("   %d\n",helpBuffer[0][i]);
            voice[voiceCNT].time += 1.0 / (double)SAMPLE_RATE;
            timeInPeriode[voiceCNT] = (timeInPeriode[voiceCNT] + 1 ) % SAMPLE_RATE;
        }
    }

    for (int i = 0; i < BUFFER_CHUNK_SIZE; i++) {
       buffers[chunkIndex][i] = helpBuffer[0][i];
       buffers[chunkIndex][i] += helpBuffer[1][i];
       buffers[chunkIndex][i] += helpBuffer[2][i];
       /*
       if (buffers[chunkIndex][i]>max) {
        max= buffers[chunkIndex][i];
       }
       if (buffers[chunkIndex][i]<min) {
        min= buffers[chunkIndex][i];
       }
       */
    }

     //   printf("max %d\t",max);
     //   printf("min %d\n",min);

} 



// Initialisiert das Audio und bereitet den Ringpuffer vor
int intSid() {
    printf("Init SID\n");

    for (int i=0;i<MAX_VOICE;i++) {
        voice[i].freq = 440;
        voice[i].attack = 0.05;
        voice[i].decay = 0.05;
        voice[i].release = 0.1;
        voice[i].dutyCycle = 0.5;
        voice[i].sustainVol = 10;
        voice[i].keyBit = 0;
        voice[i].state = MUTE;// ATTACK; // MUTE;
        voice[i].time = 0;
        voice[i].type =  NONE; // TRI; 
    }
    
    sidRegister.volume_and_filter = 0x0f; // set volume to max

    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        printf("Fehler beim Öffnen des Audioausgabegeräts.\n");
        return -1;
    }


    // Bereite alle Puffersegmente vor (Ringpuffer)
    for (int i = 0; i < BUFFER_CHUNK_COUNT; i++) {
        headers[i].lpData = (LPSTR)buffers[i];
        headers[i].dwBufferLength = BUFFER_CHUNK_SIZE * sizeof(short);
        headers[i].dwFlags = WHDR_DONE;

        if (waveOutPrepareHeader(hWaveOut, &headers[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
            printf("Fehler beim Vorbereiten des Headers %d.\n", i);
            waveOutClose(hWaveOut);
            return -1;
        }
  
    }
      for (int i = 0; i < BUFFER_CHUNK_COUNT; i++) {
        fill_buffer_with_tone(i);
        if (waveOutWrite(hWaveOut, &headers[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
            printf("Fehler beim Starten des Buffers %d.\n", i);
        }
    }

    return 0;
}

// Spielt den Ringpuffer kontinuierlich ab
void play_continuous_tone() {
    // Lade alle Puffersegmente initial
  

    if (headers[currentChunk].dwFlags & WHDR_DONE) {
        headers[currentChunk].dwFlags &= ~WHDR_DONE; // Setze Flag zurück
        fill_buffer_with_tone(currentChunk);

        if (waveOutWrite(hWaveOut, &headers[currentChunk], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
            printf("Fehler beim Schreiben des Buffers %d.\n", currentChunk);
        }
       // printf("Chunk %d\n",currentChunk);
        currentChunk = (currentChunk + 1) % BUFFER_CHUNK_COUNT;
    }
}
// Beendet die Audioausgabe und gibt Ressourcen frei
void close_audio() {
    // Alle Puffer freigeben
    for (int i = 0; i < BUFFER_CHUNK_COUNT; i++) {
        waveOutUnprepareHeader(hWaveOut, &headers[i], sizeof(WAVEHDR));
    }
    waveOutClose(hWaveOut);
}


void doSid() {

    play_continuous_tone();
//    close_audio();    
}


