
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cia.h"
#include "cpu6510.h"
#include "progloader.h"
#include "vic.h"

#define INLUDE_KEYMAP
#include "keymap.h"

int useStick = 0;

void keyMapDown(int ascii, int rawKey) {
    if ((rawKey >= 115) && (rawKey < 124)) {
        switch (rawKey) {
            case 115:  // F5
                break;
            case 116:  // F5
                break;
            case 117:  // F6
                break;
            case 118:  // F7
                break;
            case 119:  // F8
                break;
            case 120:  // F9
                useStick = 1 - useStick;
                printf("Use Joystick %d\n", useStick);
                break;
            case 122:  // F11
                break;
            case 123:  // F12
                loadPrg(rawKey);
                break;
        }
        return;
    }

    if (useStick) {
        for (int i = 0; i < sizeof(stickMappingsWParam) / sizeof(stickMappingsWParam[0]); i++) {
            if (rawKey == stickMappingsWParam[i].keycode) {
                if (i < 5) {
                    portKeyMap.stick1 |= stickMappingsWParam[i].portA1;
                } else {
                    portKeyMap.stick2 |= stickMappingsWParam[i].portB1;
                }
                return;
            }
        }

        // return;
    }

    if (portKeyMap.lifeTime <= 0) {
        if (ascii) {
            for (int i = 0; i < sizeof(keyMappings) / sizeof(keyMappings[0]); i++) {
                if (ascii == keyMappings[i].keycode) {
                    portKeyMap.lifeTime = 30000;  // 30ms
                    portKeyMap.portA1 = keyMappings[i].portA1;
                    portKeyMap.portB1 = keyMappings[i].portB1;
                    portKeyMap.portA2 = keyMappings[i].portA2;
                    portKeyMap.portB2 = keyMappings[i].portB2;
                    // printf("%02x   %02x  %02x
                    // %02x\n",portKeyMap.portA1,portKeyMap.portB1,portKeyMap.portA2,portKeyMap.portB2);
                    return;
                }
            }
        } else {
            if (rawKey) {
                for (int i = 0; i < sizeof(keyMappingsWParam) / sizeof(keyMappingsWParam[0]); i++) {
                    if (rawKey == keyMappingsWParam[i].keycode) {
                        portKeyMap.lifeTime = 30000;  // 30ms
                        portKeyMap.portA1 = keyMappingsWParam[i].portA1;
                        portKeyMap.portB1 = keyMappingsWParam[i].portB1;
                        portKeyMap.portA2 = keyMappingsWParam[i].portA2;
                        portKeyMap.portB2 = keyMappingsWParam[i].portB2;
                        //   printf("%02x   %02x  %02x
                        //   %02x\n",portKeyMap.portA1,portKeyMap.portB1,portKeyMap.portA2,portKeyMap.portB2);
                        return;
                    }
                }
            }
        }
    }
}

void keyMapUp(int rawKey) {
    if ((rawKey >= 116) && (rawKey < 124)) {
        switch (rawKey) {
            case 116:  // F5
                break;
            case 117:  // F6
                break;
            case 118:  // F7
                break;
            case 119:  // F8
                break;
            case 120:  // F9
                break;
            case 122:  // F11
                break;
            case 123:  // F12
                break;
        }
    }

    if (useStick) {
        for (int i = 0; i < sizeof(stickMappingsWParam) / sizeof(stickMappingsWParam[0]); i++) {
            if (rawKey == stickMappingsWParam[i].keycode) {
                if (i < 5) {
                    portKeyMap.stick1 &= ~stickMappingsWParam[i].portA1;
                } else {
                    portKeyMap.stick2 &= ~stickMappingsWParam[i].portB1;
                }
                return;
            }
        }
    }
}
