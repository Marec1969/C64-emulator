
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "cpurunner.h"
#include "mainWin.h"
#include "vic.h"
#include "floppy1570.h"

// Function prototypes
void* mainLoop(void* arg);     // Thread function for the main loop
void* messageLoop(void* arg);  // Thread function for message processing

// Global variable to control threads
volatile int running = 1;

extern void terminateWindow(void);

void  intSid(void);
#if 1
void  doSid(void);

void CALLBACK TimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
     doSid();
}
#endif

void mainStop(int from) {
    running = 0;
    if (from == 0) {
        terminateWindow();
    }
}

int mainRunning(void) { return running; }

// Main function
int main() {
    pthread_t mainThread, msgThread;
    printf("Main start\n");

    intSid();

    MMRESULT timerID = timeSetEvent(
        10,                // Intervall von 10 ms
        5,                // Auflösung von 1 ms
        TimerCallback,     // Callback-Funktion
        0,                 // Benutzerdefinierte Daten
        TIME_PERIODIC      // Periodischer Timer
    );

    if (timerID == 0) {
        printf("Multimedia-Timer konnte nicht erstellt werden.\n");
        return 1;
    }

//    Sleep(5010); // 5 Sekunden warten, damit der Timer aktiv ist

#if 1
    cpuRunnerInit();
    init1570();

    // Creating the thread for the main loop
    if (pthread_create(&mainThread, NULL, mainLoop, NULL)) {
        fprintf(stderr, "Error creating main loop thread");
        return 1;
    }

    // Creating the thread for message processing
    if (pthread_create(&msgThread, NULL, messageLoop, NULL)) {
        fprintf(stderr, "Error creating message loop thread");
        return 1;
    }

    // Waiting for the termination of the threads
    pthread_join(mainThread, NULL);  // Waiting for the main loop thread
    pthread_join(msgThread, NULL);   // Waiting for the message processing thread

#endif
    timeKillEvent(timerID); // Timer beenden
    return 0;
}

// Thread function for the main loop
void* mainLoop(void* arg) {
    printf("The main loop is running...");
    cpuRunnerDo();
    return NULL;  // End thread
}

// Dummy thread function for message processing
void* messageLoop(void* arg) {
    runMainWindow();
    return NULL;  // End thread
}
