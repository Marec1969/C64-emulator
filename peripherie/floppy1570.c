#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "floppy1570.h"
#include "cia.h"

#define MAXCHANNELDATA 256
#define MAXCHANNEL 15

typedef struct {
ChannelState state;    
uint16_t send;
uint16_t dataCnt;
uint8_t data[MAXCHANNELDATA];
} Channel_t;

// Simulierte Leitungen
bool serClk = true;
bool serDat = true;
bool serAtn = false;

bool serClk1570 = true;
bool serDat1570 = true;


// Globale Zeit in Mikrosekunden
uint32_t usTime = 0;

uint8_t addr1570 = 8;


TranmitState transmitState = TRANSMIT_IDLE;
TranmitState talkState = TRANSMIT_IDLE;
DeviceState deviceState = DEVICE_IDLE;

// Globale Variablen
uint8_t received_data = 0;    // Puffer für empfangene Daten
uint8_t command = 0;          // Aktuelles Kommando
uint8_t data_to_send[256];    // Simulierte Daten (z. B. Verzeichnis)
size_t data_length = 0;       // Länge der zu sendenden Daten
size_t data_index = 0;        // Index für gesendete Daten
int bit_index = 0;            // Bit-Zähler
uint64_t last_action_time = 0;  // Zeitpunkt der letzten Aktivität

Channel_t channel[MAXCHANNEL];

int16_t aktiveChannel=-1;

static void update_time(void) {
    extern uint32_t clkCount;
    usTime = clkCount;
}


bool    serClko, serClk1570o,serClkC64o;
bool    serDato,serDat1570o,serDatC64o;


void updateDataline(void) {
    serClk = serClk1570 || serClkC64;
    serDat = serDat1570 || serDatC64;

    if ( serClko !=  serClk || serClk1570o !=  serClk1570 ||  serClkC64o != serClkC64 ||  serDato != serDat ||  serDat1570o != serDat1570 ||   serDatC64o !=  serDatC64) {
        printf("[%llu us] clk %d (%d %d) data %d (%d %d)\t\t ATN %d\n", usTime, serClk , serClk1570 , serClkC64, serDat , serDat1570 , serDatC64, serAtn);

    }

    serClko =  serClk;
    serClk1570o =  serClk1570;
    serClkC64o =   serClkC64;
    serDato =   serDat;
    serDat1570o =   serDat1570;
    serDatC64o =  serDatC64;



}

bool check_timeout(uint64_t start_time, uint64_t timeout_us) {
    update_time();
    return (usTime - start_time) >= timeout_us;
}


void  setCommand(uint8_t receivedCommand) {
uint8_t receivedAddr = receivedCommand & 0x0f;
    receivedCommand = receivedCommand & 0xf0;
printf("set Command %02x   %02x\n",receivedCommand,receivedAddr);
    switch(receivedCommand ) {
        case 0x20:
            if (receivedAddr==addr1570) {
                deviceState = DEVICE_LISTEN;
            }
        break;
        case 0x30:
                deviceState = DEVICE_IDLE;
        break;
        case 0x40:
            if (receivedAddr==addr1570) {
                talkState = TRANSMIT_IDLE;
                deviceState = DEVICE_TALK;
            }
        break;
        case 0x50:
                deviceState = DEVICE_IDLE;
        break;
        case 0x60:  // OPEN CHANNEL          
            if (deviceState == DEVICE_LISTEN) {
                aktiveChannel = receivedAddr;
                channel[receivedAddr].dataCnt=0;
                channel[receivedAddr].send = 0;
                channel[receivedAddr].state = CHANNEL_OPEN;
            }
        break;
        case 0xE0: // CLOSE
            if (deviceState == DEVICE_LISTEN) {
                channel[receivedAddr].dataCnt=0;
                channel[receivedAddr].send = 0;
                channel[receivedAddr].state = CHANNEL_CLOSE;
            }
            aktiveChannel = -1;
        break;
        case 0xF0: // OPEN 
            if (deviceState == DEVICE_LISTEN) {
                aktiveChannel = receivedAddr;
                channel[receivedAddr].dataCnt=0;
                channel[receivedAddr].send = 0;
                channel[receivedAddr].state = CHANNEL_OPEN;
            }
        break;
        default:
            printf("Command not Valid -> exit\n");
            exit(1);
    }
}


int16_t fetchData(int16_t channelNr) {
    int16_t data = -1;
    if ((deviceState == DEVICE_TALK) && (channelNr>=0) && (channel[channelNr].state == CHANNEL_OPEN)){
        if (channel[channelNr].dataCnt>0) {
            data = channel[channelNr].data[channel[channelNr].send];
            printf("\t\t\t\t\t\t\t\t\tfetch Byte %d  %d\n",channel[channelNr].send,data);
            channel[channelNr].send++;
            channel[channelNr].dataCnt--;
        }
    }
    return data;
}

bool dataEmpty(int16_t channelNr) {
    if ((deviceState == DEVICE_TALK) && (channelNr>=0) && (channel[channelNr].state == CHANNEL_OPEN)){
        if (channel[channelNr].dataCnt>0) {
            return false;
        }
    }
    return true;
}


bool lastData(int16_t channelNr) {
    if ((deviceState == DEVICE_TALK) && (channelNr>=0) && (channel[channelNr].state == CHANNEL_OPEN)){
        if (channel[channelNr].dataCnt==1) {
            return true;
        }
    }
    return false;
}


void storeData(int16_t channelNr,uint8_t data) {
    if ((deviceState == DEVICE_LISTEN) && (channelNr>=0) && (channel[channelNr].state == CHANNEL_OPEN)){
        channel[channelNr].data[channel[channelNr].dataCnt] = data;
        channel[channelNr].dataCnt++;
        printf("\t\t\t\t\t\t\t\t\t Storre Byte %d  %d\n",channel[channelNr].dataCnt,data);
    }
}



void device1570Update(void) {
    static uint8_t received_byte = 0;  // Zwischenspeicher für das empfangene Byte
    static uint8_t received_command = 0;  // Zwischenspeicher für das empfangene Byte
    static int bit_index = 0;         // Bit-Index
    static bool eoi_detected = false; // Kennzeichnung, ob EOI erkannt wurde
    static uint64_t last_action_time = 0;  // Zeitpunkt der letzten Aktion
    static bool waitEdge = false;
    static bool atnOld = false;
    static DeviceState oldDeviceState=DEVICE_IDLE;
    static int16_t sendByte=-1;

    static int32_t dbgWait=0;
    
    update_time();

    if (dbgWait) {
        // dbgWait--;
        if (dbgWait==10) {
          exit(1);
        }
    }

    if ((atnOld==false) && (serAtn==true)) { // Host switched to "attention" -> go to idle-state and wait
        // if (transmitState!=TRANSMIT_WAIT_FOR_DATA_1)   
        printf("[%llu us] Host switched to attention -> go idle\n", usTime);
        last_action_time = usTime;
        transmitState = TRANSMIT_IDLE;
        // dbgWait=5000;
    }

    atnOld = serAtn;


    if ((serAtn==false) && (deviceState == DEVICE_TALK)) { 
        switch (talkState) {
            case TRANSMIT_IDLE:  //Step 0
                // Step Zero: Beide Leitungen werden gehalten
                serClk1570 = false;
                serDat1570 = false;
                updateDataline();
                eoi_detected = false;                
                talkState = TRANSMIT_READY_FOR_DATA_1;
                break;

            case TRANSMIT_READY_FOR_DATA_1: // step 1            
                if (serClk==false) {  // Step 1: Talker signalisiert "Ready to Send"
                    printf("[%llu us] Clock auf FALSE. go TRANSMIT_READY_FOR_DATA_2.\n", usTime);
                    serClk1570 = true;
                    serDat1570 = false;
                    updateDataline();
                    last_action_time = usTime;
                    talkState = TRANSMIT_READY_FOR_DATA_2;            
                }
                break;

            case TRANSMIT_READY_FOR_DATA_2: // step 2
                if (check_timeout(last_action_time, 100)) {  // Timeout 80us
                    printf("[%llu us] wait 80us auf go send Data.\n", usTime);
                    serClk1570 = false;
                    updateDataline();
                    last_action_time = usTime;                    
                    if (lastData(aktiveChannel)) {
                        talkState = TRANSMIT_SET_EOI;
                        eoi_detected = false;
                    } else {
                        talkState = TRANSMIT_WAIT_ACK;
                    }
                }
                
                break;

            case TRANSMIT_SET_EOI:
                if (check_timeout(last_action_time, 250)) {  // Timeout 250us
                    if (serDat) {
                        printf("[%llu us] EOI ak true.\n", usTime);
                        eoi_detected = true;
                    } 
                    if (!serDat && eoi_detected) {
                        printf("[%llu us] EOI ak low.\n", usTime);
                        talkState = TRANSMIT_WAIT_ACK;
                        serClk1570 = true;
                        updateDataline();
                    }
                }
                break;

            case TRANSMIT_WAIT_EOI:
                break;

            case TRANSMIT_WAIT_ACK:
                if (!serDat) {
                    printf("[%llu us] start write ack.\n", usTime);
                    last_action_time = usTime;
                    bit_index = 0;
                    talkState = TRANSMIT_SENDING;
                }
                break;


            case TRANSMIT_SENDING:           
                
//                if (check_timeout(last_action_time, lastData(aktiveChannel) ? 250 : 60)) {  // Timeout 20us
                if (check_timeout(last_action_time, 60)) {  // Timeout 20us
                    if (serClk1570 == true) {
                        if (bit_index==0) {
                            sendByte = fetchData(aktiveChannel);
                            printf("fetch Byte %04x\n",sendByte);
                        }
                        if (sendByte & ( 1 << bit_index)) {
                            serDat1570 = true;
                        } else {
                            serDat1570 = false;
                        }
                        bit_index++;
                        serClk1570 = false;
                    } else {
                        serClk1570 = true;
                        if (bit_index==8) {
                            serDat1570 = false;
                               talkState = TRANSMIT_ACKNOWLEDGE;
                        }
                    }
                    updateDataline();
                    last_action_time = usTime;
                }

                break;

            case TRANSMIT_ACKNOWLEDGE:
                if (check_timeout(last_action_time, 1000)) {  // 1000 µs Verzögerung für ACK
                    printf("[%llu us] ACK gesendet -> wait next byte\n", usTime);
                    last_action_time = usTime;
                    exit(1);
                }
                if (serDat==true) {
                    if ( dataEmpty(aktiveChannel) ) {                    
                        printf("Send done \n");
                        deviceState = DEVICE_IDLE;
                        talkState = TRANSMIT_IDLE;
                        transmitState = TRANSMIT_RETURN_LISTEN;
                    } else {
                        printf("Send next \n");
                        talkState = TRANSMIT_READY_FOR_DATA_2;
                    }
                }
                break;

            case TRANSMIT_RETURN_LISTEN:

                break;
            default:
                printf("[%llu us] Unbekannter Zustand: Zurueck zu DEVICE_IDLE.\n", usTime);
                talkState = TRANSMIT_IDLE;
                break;
        }

    } else {

        if (check_timeout(last_action_time, 1000)) {  // Timeout 1000us // in device mode we lock for timeouts
            if (transmitState!=TRANSMIT_WAIT_FOR_DATA_1)  printf("[%llu us] empfang timeout -> go idle\n", usTime);
            last_action_time = usTime;
            transmitState = TRANSMIT_IDLE;
            // dbgWait=5000;
        }

        switch (transmitState) {
            case TRANSMIT_IDLE:  //Step 0
                // Step Zero: Beide Leitungen werden gehalten
                serClk1570 = false;
                serDat1570 = true;
                updateDataline();
                eoi_detected = false;
                transmitState = TRANSMIT_WAIT_FOR_DATA_1;
                break;

            case TRANSMIT_WAIT_FOR_DATA_1: // step 1
                if (serClk==false) {  // Step 1: Talker signalisiert "Ready to Send"
                    printf("[%llu us] Clock auf FALSE. go DEVICE_WAIT_FOR_COMMAND2.\n", usTime);
                    serDat1570 = false;
                    updateDataline();
                    last_action_time = usTime;
                    transmitState = TRANSMIT_WAIT_FOR_DATA_2;            
                }
                break;

            case TRANSMIT_WAIT_FOR_DATA_2: // step 2
                if (serClk==true) {  // Step 2: Talker signalisiert "Ready to Send"
                    printf("[%llu us] Clock auf True. Empfangen eines Bytes beginnt.\n", usTime);
                    bit_index = 0;
                    received_byte = 0;
                    waitEdge = false;
                    serDat1570 = false;
                    updateDataline();
                    last_action_time = usTime;
                    transmitState = TRANSMIT_RECEIVING;
                }
                
                if (check_timeout(last_action_time, 200)) {  // Timeout 200us
                    printf("[%llu us] Timeout beim Warten auf Clock.Ab zu DEVICE_EOI.\n", usTime);
                    serDat1570 = true;
                    updateDataline();
                    last_action_time = usTime;
                    transmitState = TRANSMIT_EOI;
                }
                
                break;

            case TRANSMIT_RECEIVING:                        
                if (serClk) {
                    if (bit_index==8) {
                        if (serAtn) {
                            received_command = received_byte;
                            setCommand(received_command);
                        } else {
                            storeData(aktiveChannel,received_byte);
                            if (deviceState==DEVICE_LISTEN) {
                                printf("[%llu us] Byte empfangen: 0x%02X    command = 0x%02X\n", usTime, received_byte,received_command);
                            }
                        }
                        serDat1570 = true;
                        updateDataline();
                        transmitState = TRANSMIT_ACKNOWLEDGE;
                        last_action_time = usTime;
                        break;
                    }

                    waitEdge = true;
                }
                if (waitEdge && !serClk ) {  // Step 3: Empfange Bit
                    waitEdge = false;
                    if (bit_index < 8) {
                        if (serDat==false) {
                            received_byte |= (1 << bit_index);
                        }
                        printf("[%llu us] Empfangenes Bit %d: %d\n", usTime, bit_index, serDat ? 1 : 0);
                        bit_index++;
                        last_action_time = usTime;  // Aktualisierung der letzten Aktivität
                    } else {
                        printf("But Error -> exit\n");
                        exit(1);
                    }
                }
                break;

            case TRANSMIT_ACKNOWLEDGE:
                if (check_timeout(last_action_time, 70)) {  // 40 µs Verzögerung für ACK
                    if (eoi_detected) {
                        transmitState = TRANSMIT_IDLE;
                        printf("[%llu us] ACK gesendet -> go idle\n", usTime);
                    } else {
                        transmitState = TRANSMIT_WAIT_FOR_DATA_1;
                        printf("[%llu us] ACK gesendet -> wait next byte\n", usTime);
                    }
                    last_action_time = usTime;
                }
                break;

            case TRANSMIT_EOI:
                if (check_timeout(last_action_time, 70)) {  // 60 µs EOI halten
                    printf("[%llu us] EOI-ACK abgeschlossen. Wechsel zu DEVICE_RECEIVING.\n", usTime);
                    dbgWait++;
                    serDat1570 = false;
                    updateDataline();
                    eoi_detected = true;
                    transmitState = TRANSMIT_WAIT_FOR_DATA_2;
                    last_action_time = usTime;
                }
                break;
            default:
                printf("[%llu us] Unbekannter Zustand: Zurueck zu DEVICE_IDLE.\n", usTime);
                transmitState = TRANSMIT_IDLE;
                break;
        }
    }

    oldDeviceState = deviceState;
}


void init1570(void) {


    // Initialisierung: Setze Zeit und Zustand
    update_time();
    transmitState = TRANSMIT_IDLE;

}
