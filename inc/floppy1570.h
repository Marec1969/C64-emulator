#ifndef FLOPPY1570_H
#define FLOPPY1570_H

#include <stdint.h>
#include <stdbool.h>

// Simulierte Leitungen
extern bool serClk;
extern bool serDat;
extern bool serAtn;

// Globale Zeit in Mikrosekunden
extern uint32_t usTime;

// Gerät-Zustände
typedef enum {
    TRANSMIT_IDLE,           
    TRANSMIT_WAIT_FOR_DATA_1,
    TRANSMIT_WAIT_FOR_DATA_2,
    TRANSMIT_RECEIVING,        // Empfängt ein Byte
    TRANSMIT_ACKNOWLEDGE,       // Bestätigt Empfang
    TRANSMIT_EOI,               // Verarbeitet ein End-of-Indicator (EOI) Signal
    TRANSMIT_READY_FOR_DATA_1,
    TRANSMIT_READY_FOR_DATA_2,
    TRANSMIT_WAIT_ACK,
    TRANSMIT_SET_EOI,
    TRANSMIT_WAIT_EOI,
    TRANSMIT_RETURN_LISTEN,
    TRANSMIT_SENDING        // send a Byte
} TranmitState;


typedef enum {
    DEVICE_IDLE,           
    DEVICE_LISTEN,           
    DEVICE_TALK  
} DeviceState;

typedef enum {
    CHANNEL_OPEN,           
    CHANNEL_CLOSE           
} ChannelState;



void updateDataline(void);
void device1570Update(void);
void init1570(void);

#endif
