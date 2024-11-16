#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sid.h"

// Definition der Enums soundType und soundState (beispielhaft)
typedef enum {
    SOUND_TYPE_1,
    SOUND_TYPE_2
} soundType;

typedef enum {
    SOUND_STATE_IDLE,
    SOUND_STATE_PLAYING
} soundState;


#define MAX_NOTES 200

voice_t song[MAX_VOICE][MAX_NOTES];
uint16_t noteCnt[MAX_VOICE];


void putNoteToSong(int voiceNr) {
    int nr = noteCnt[voiceNr];
    extern uint32_t clkCount;

    if (nr<MAX_NOTES) {

        song[voiceNr][nr].freq = voice[voiceNr].freq;
        song[voiceNr][nr].dutyCycle = voice[voiceNr].dutyCycle;
        song[voiceNr][nr].sync = voice[voiceNr].sync;
        song[voiceNr][nr].attack = voice[voiceNr].attack;
        song[voiceNr][nr].decay = voice[voiceNr].decay;
        song[voiceNr][nr].sustainVol = voice[voiceNr].sustainVol;
        song[voiceNr][nr].release = voice[voiceNr].release;
        song[voiceNr][nr].type = voice[voiceNr].type;
        song[voiceNr][nr].keyBit = voice[voiceNr].keyBit;

        if (voice[voiceNr].keyBit) {
            if (song[voiceNr][nr].time!=0.0) {
                printf("Error set Note Start\n");
                exit(1);
            }
            song[voiceNr][nr].time = (double)clkCount*1e-6;
        } else {
            if (song[voiceNr][nr].time==0.0) {
                printf("Error set Note End\n");
                // exit(1);
            }
            song[voiceNr][nr].endTime = (double)clkCount*1e-6;
            noteCnt[voiceNr]++;
        }
    }

}




// Funktion zum Vergleichen von zwei voice_t basierend auf der "time"
int compareVoiceTime(const void *a, const void *b) {
    voice_t *voiceA = (voice_t *)a;
    voice_t *voiceB = (voice_t *)b;
    if (voiceA->time > voiceB->time) return 1;
    if (voiceA->time < voiceB->time) return -1;
    return 0;
}

int saveSong() {

    // Array für gültige Stimmen vorbereiten
    voice_t validVoices[MAX_VOICE * MAX_NOTES];
    int validCount = 0;

    // Filtern von gültigen Stimmen
    for (int i = 0; i < MAX_VOICE; i++) {
        for (int j = 0; j < MAX_NOTES; j++) {
            if (song[i][j].time > 0) {
                song[i][j].keyBit = i+1;
                validVoices[validCount++] = song[i][j];
            }
        }
    }

    // Sortieren der gültigen Stimmen nach "time"
    qsort(validVoices, validCount, sizeof(voice_t), compareVoiceTime);

    // Schreiben der sortierten Daten in eine Datei
    FILE *file = fopen("sorted_song_data.txt", "w");
    if (file == NULL) {
        perror("Fehler beim Öffnen der Datei");
        return 1;
    }

    for (int i = 0; i < validCount; i++) {
        fprintf(file, "time: %.3f, time: %.3f, length: %.3f, freq: %.2f, dutyCycle: %.2f, ring: %u, sync: %u, attack: %.3f, decay: %.3f, sustainVol: %u, release: %.3f, keyBit: %u, type: %d, state: %d\n",
                validVoices[i].time,
                validVoices[i].endTime,
                validVoices[i].endTime-validVoices[i].time,
                validVoices[i].freq * 0.01,
                validVoices[i].dutyCycle,
                validVoices[i].ring,
                validVoices[i].sync,
                validVoices[i].attack,
                validVoices[i].decay,
                validVoices[i].sustainVol,
                validVoices[i].release,
                validVoices[i].keyBit,
                validVoices[i].type,
                validVoices[i].state);
    }

    fclose(file);
    printf("Die sortierten Daten wurden in 'sorted_song_data.txt' gespeichert.\n");
    return 0;
}
