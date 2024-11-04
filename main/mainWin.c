
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "vic.h"

#include "cpuCnt.h"
#include "main.h"

// Initialization of colors
RGBQUAD farben[] = {
    {0x00, 0x00, 0x00, 0x00},  // Black
    {0xFF, 0xFF, 0xFF, 0x00},  // White
    {0x00, 0x00, 0x88, 0x00},  // Red
    {0xEE, 0xFF, 0xAA, 0x00},  // Turquoise
    {0xCC, 0x44, 0xCC, 0x00},  // Purple
    {0x55, 0xCC, 0x00, 0x00},  // Green
    {0xAA, 0x00, 0x00, 0x00},  // Blue
    {0x77, 0xEE, 0xEE, 0x00},  // Yellow
    {0x55, 0x88, 0xDD, 0x00},  // Orange
    {0x00, 0x44, 0x66, 0x00},  // Brown
    {0x77, 0x77, 0xFF, 0x00}, // Light Red
    {0x33, 0x33, 0x33, 0x00}, // Dark Gray
    {0x77, 0x77, 0x77, 0x00}, // Medium Gray
    {0x66, 0xFF, 0xAA, 0x00}, // Light Green
    {0xFF, 0x88, 0x00, 0x00}, // Light Blue
    {0xBB, 0xBB, 0xBB, 0x00},  // Light Gray

// Double the color set so that bit 4 (0x10) can be used for background collision detection
    {0x00, 0x00, 0x00, 0x00},  // Black
    {0xFF, 0xFF, 0xFF, 0x00},  // White
    {0x00, 0x00, 0x88, 0x00},  // Red
    {0xEE, 0xFF, 0xAA, 0x00},  // Turquoise
    {0xCC, 0x44, 0xCC, 0x00},  // Purple
    {0x55, 0xCC, 0x00, 0x00},  // Green
    {0xAA, 0x00, 0x00, 0x00},  // Blue
    {0x77, 0xEE, 0xEE, 0x00},  // Yellow
    {0x55, 0x88, 0xDD, 0x00},  // Orange
    {0x00, 0x44, 0x66, 0x00},  // Brown
    {0x77, 0x77, 0xFF, 0x00}, // Light Red
    {0x33, 0x33, 0x33, 0x00}, // Dark Gray
    {0x77, 0x77, 0x77, 0x00}, // Medium Gray
    {0x66, 0xFF, 0xAA, 0x00}, // Light Green
    {0xFF, 0x88, 0x00, 0x00}, // Light Blue
    {0xBB, 0xBB, 0xBB, 0x00}  // Light Gray

};



typedef struct {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[32];
} BITMAPINFO_32COLORS;

BITMAPINFO_32COLORS bmiColor;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND windowHandle=0;

BITMAPINFO* bmi;

//static uint8_t* canvas=0;

static uint8_t canvas[PAL_B_Y][PAL_B_X];

void   windowsUpdateScreen(uint8_t * screenPtr) {
    if (windowHandle)  {
        // canvas = screenPtr;
        memcpy(&canvas[0][0],screenPtr,PAL_B_Y*PAL_B_X);
        RedrawWindow(windowHandle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Jetzt sofort neu zeichnen               
    }
}

void terminateWindow(void) {
    SendMessage(windowHandle, WM_CLOSE, 0, 0);
}


// Fensterprozedur, die Nachrichten verarbeitet
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
    case WM_DESTROY:
        mainStop(1);
        PostQuitMessage(0);        
        return 0;

    case WM_PAINT: 
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (canvas) {
            SetDIBitsToDevice(hdc, 0, 0,  PAL_B_X, PAL_B_Y, 0, 0, 0, PAL_B_Y, &canvas[0][0], bmi, DIB_RGB_COLORS);          
//            SetDIBitsToDevice(hdc, 0, 0,  PAL_B_X, PAL_B_Y, 0, 0, 0, PAL_B_Y, canvas, bmi, DIB_RGB_COLORS);          
        }
        EndPaint(hwnd, &ps);    
    break;
    case WM_KEYDOWN:

        // Array für die ASCII-Konvertierung
        char asciiChar[2];
        BYTE keyboardState[256];
        GetKeyboardState(keyboardState);
        
        // Konvertiere den virtuellen Tastencode (wParam) in ASCII
        int asciiLength = ToAscii(wParam, MapVirtualKey(wParam, MAPVK_VK_TO_VSC), keyboardState, (LPWORD)asciiChar, 0);
        
        if ((asciiLength == 1) && (asciiChar[0]>=32)) {  // Wenn es genau ein Zeichen ist (kein Sonderzeichen)
            printf("ASCII-Code: %02X\n", asciiChar[0]);  // ASCII-Code ausgeben
        } else {
            // asciiChar[0]=0;
            printf(" Wpara %d\n",wParam);
        }

        keyMapDown(asciiChar[0],wParam);
        return 0;

        case WM_KEYUP:
            keyMapUp(wParam);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void initBMI() {
    ZeroMemory(&bmiColor, sizeof(BITMAPINFO_32COLORS));
    bmiColor.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    bmiColor.bmiHeader.biWidth = PAL_B_X;
    bmiColor.bmiHeader.biHeight = -PAL_B_Y; // Negative Höhe für Top-Down Bitmap
    bmiColor.bmiHeader.biPlanes = 1;
    bmiColor.bmiHeader.biBitCount = 8; // 24 Bit für RGB
    bmiColor.bmiHeader.biCompression = BI_RGB;

    // Farbpalette in der bmiColors festlegen
    for (int i = 0; i < 32; ++i) {
        bmiColor.bmiColors[i] = farben[i];
    }

    bmi = (BITMAPINFO *) &bmiColor;

}

// Main function
int runMainWindow(void) {
    // Fensterklassenstruktur initialisieren
    const char CLASS_NAME[] = "SimpleWindowClass";

    WNDCLASS wc = { 0 };

    // prepareAllBitmaps();

    wc.lpfnWndProc = WindowProc; // Fensterprozedur
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    // Fensterklasse registrieren
    RegisterClass(&wc);

    initBMI();

    // Zielgröße der Client Area
    int desiredWidth = PAL_B_X;
    int desiredHeight = PAL_B_Y;

    // Struktur, um die Größe des Fensters zu speichern
    RECT windowRect = { 0, 0, desiredWidth, desiredHeight };

    // Berechne die äußere Fenstergröße, basierend auf der gewünschten Clientgröße
    AdjustWindowRectEx(
        &windowRect,          // RECT, der die gewünschte Größe und Position enthält
        WS_OVERLAPPEDWINDOW,  // Fensterstil (muss derselbe wie bei CreateWindowEx sein)
        FALSE,                // Hat das Fenster ein Menü?
        0                     // Erweiterter Fensterstil (muss derselbe wie bei CreateWindowEx sein)
    );

    // Berechnete Breite und Höhe des gesamten Fensters
    int adjustedWidth = windowRect.right - windowRect.left;
    int adjustedHeight = windowRect.bottom - windowRect.top;

    // Ein Fenster erstellen mit den berechneten Werten für die Breite und Höhe
    HWND hwnd = CreateWindowEx(
        0,                              // Erweiterter Fensterstil
        CLASS_NAME,                     // Fensterklasse
        "Simple Graphics Window",        // Fenstertitel
        WS_OVERLAPPEDWINDOW,            // Fensterstil
        1000, 600,    // Anfangsposition
        adjustedWidth, adjustedHeight,   // Berechnete Breite und Höhe
        NULL,                           // Übergeordnetes Fenster
        NULL,                           // Menü
        wc.hInstance,                   // Instanzhandle
        NULL                            // Zusätzliche anwendungsspezifische Daten
    );

    if (hwnd == NULL) {
        return 0;
    }

    windowHandle = hwnd;

    // Fenster anzeigen
    ShowWindow(hwnd, SW_SHOW);

    // Nachrichten-Schleife
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

 