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

bool stdebug=false;
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

//uint8_t testData[12] =           {11, 8, 10, 0, 153, 32, 34, 65, 34, 0, 0, 0 };
// uint8_t testData[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
uint8_t testData[14] = { 1, 8, 11, 8, 10, 0, 153, 32, 34, 65, 34, 0, 0, 0 };
#if 0
0800  00 0c 08 0a 00 41 20 b2 20 32 30 00 17 08 14 00 42 20 b2 20 33 30 00 25 08 1e 00 43 20 b2 20 41 			     A ² 20     B ² 30 %   C ² A
0820  20 ac 20 42 00 3c 08 28 00 99 20 22 45 52 47 45 42 4e 49 53 53 20 3d 20 22 2c 43 00 00 00 41 00 			 ¬ B < ( ™ "ERGEBNISS = ",C   A 
0840  85 20 00 00 00 42 00 85 70 00 00 00 43 00 8a 16 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 			…    B …p   C Š                 
#endif 

bool    serClko, serClk1570o,serClkC64o;
bool    serDato,serDat1570o,serDatC64o;
bool    serAtno;


void updateDataline(void) {
    serClk = serClk1570 || serClkC64;
    serDat = serDat1570 || serDatC64;
#if 0
    if ( serClko !=  serClk || serClk1570o !=  serClk1570 ||  serClkC64o != serClkC64 ||  serDato != serDat ||
      serDat1570o != serDat1570 ||   serDatC64o !=  serDatC64 || serAtno != serAtn ) {
        printf("[%llu us] clk %d (%d %d) data %d (%d %d)\t\t ATN %d\n", usTime, serClk , serClk1570 , serClkC64, serDat , serDat1570 , serDatC64, serAtn);

    }

    serClko =  serClk;
    serClk1570o =  serClk1570;
    serClkC64o =   serClkC64;
    serDato =   serDat;
    serDat1570o =   serDat1570;
    serDatC64o =  serDatC64;
    serAtno = serAtn;
#endif

}

bool check_timeout(uint64_t start_time, uint64_t timeout_us) {
    update_time();
    return (usTime - start_time) >= timeout_us;
}


void  setCommand(uint8_t receivedCommand) {
uint8_t receivedAddr = receivedCommand & 0x0f;
    receivedCommand = receivedCommand & 0xf0;
    if (stdebug)  printf("set Command %02x   %02x\n",receivedCommand,receivedAddr);
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
            channel[0].dataCnt  = sizeof(testData);
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
            data = testData[channel[channelNr].send];
            if (stdebug) printf("\t\t\t\t\t\t\t\t\tfetch Byte %d  %d\n",channel[channelNr].dataCnt,data);
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
        if (stdebug)  printf("\t\t\t\t\t\t\t\t\t Storre Byte %d  %d\n",channel[channelNr].dataCnt,data);
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
         //  exit(1);
        }
    }

#if 1
    if ((atnOld==false) && (serAtn==true)) { // Host switched to "attention" -> go to idle-state and wait
        // if (transmitState!=TRANSMIT_WAIT_FOR_DATA_1)   
        if (stdebug) printf("[%llu us] Host switched to attention -> go idle\n", usTime);
        last_action_time = usTime;
        transmitState = TRANSMIT_IDLE;
        // dbgWait=5000;
    }

    atnOld = serAtn;
#endif

if (serAtn) {
        switch (transmitState) {
            case TRANSMIT_IDLE:  //Step 0
                // Step Zero: Beide Leitungen werden gehalten
                if (serClk==true)  { // we will wait fo ridle
                    transmitState = TRANSMIT_WAIT_FOR_DATA_1;
                   // printf("[%llu us] TRANSMIT_IDLE .\n", usTime);
                }
                serClk1570 = false;
                serDat1570 = true;
                updateDataline();
                break;

            case TRANSMIT_WAIT_FOR_DATA_1: // step 1
                if (serClk==false) {  // Step 1: Talker signalisiert "Ready to Send"
                    if (stdebug) printf("[%llu us] Clock auf FALSE. go DEVICE_WAIT_FOR_COMMAND2.\n", usTime);
                    serDat1570 = false;
                    updateDataline();
                    last_action_time = usTime;
                    transmitState = TRANSMIT_WAIT_FOR_DATA_2;            
                }
                break;

            case TRANSMIT_WAIT_FOR_DATA_2: // step 2
                if (serClk==true) {  // Step 2: Talker signalisiert "Ready to Send"
                   if (stdebug)  printf("[%llu us] Clock auf True. Empfangen eines Bytes beginnt.\n", usTime);
                    bit_index = 0;
                    received_byte = 0;
                    waitEdge = false;
                    serDat1570 = false;
                    updateDataline();
                    last_action_time = usTime;
                    transmitState = TRANSMIT_RECEIVING;
                }
                                
                break;

            case TRANSMIT_RECEIVING:                        
                if (serClk) {
                    if (bit_index==8) {
                        received_command = received_byte;
                        setCommand(received_command);
                        serDat1570 = true;
                        updateDataline();
                        transmitState = TRANSMIT_IDLE;
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
                        // printf("[%llu us] Empfangenes Bit %d: %d\n", usTime, bit_index, serDat ? 1 : 0);
                        bit_index++;
                        last_action_time = usTime;  // Aktualisierung der letzten Aktivität
                    } else {
                        printf("But Error -> exit\n");
                        exit(1);
                    }
                }
                break;

            default:
                printf("[%llu us] Unbekannter Zustand: Zurueck zu DEVICE_IDLE.\n", usTime);
                transmitState = TRANSMIT_IDLE;
                break;
        }
    }


    if ((deviceState == DEVICE_TALK) && (serAtn==false)) {
        // exit(1);
        switch (talkState) {
            case TRANSMIT_IDLE:  //Step 0
                // Step Zero: Beide Leitungen werden gehalten
                serClk1570 = true;
                serDat1570 = false;
                updateDataline();
                eoi_detected = false;     
                if ( !dataEmpty(aktiveChannel)) {
                    talkState = TRANSMIT_READY_FOR_DATA_1;
                }
                break;

            case TRANSMIT_READY_FOR_DATA_1: // step 1            
                if (stdebug) printf("[%llu us] Clock auf FALSE. go TRANSMIT_READY_FOR_DATA_2.\n", usTime);
                serClk1570 = true;
                serDat1570 = false;
                updateDataline();
                last_action_time = usTime;
                talkState = TRANSMIT_READY_FOR_DATA_2;            
                break;

            case TRANSMIT_READY_FOR_DATA_2: // step 2
                if (check_timeout(last_action_time, 100)) {  // Timeout 80us
                    if (stdebug) printf("[%llu us] wait 80us auf go send Data.\n", usTime);
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
                        if (stdebug) printf("[%llu us] EOI ak true.\n", usTime);
                        eoi_detected = true;
                    } 
                    if (!serDat && eoi_detected) {
                        if (stdebug) printf("[%llu us] EOI ak low.\n", usTime);
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
                    if (stdebug) printf("[%llu us] start write ack.\n", usTime);
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
                            if (stdebug) printf("fetch Byte %04x\n",sendByte);
                        }
                        if (sendByte & ( 1 << bit_index)) {
                            serDat1570 = false;
                        } else {
                            serDat1570 = true;
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
                    if (stdebug) printf("[%llu us] ACK gesendet -> wait next byte\n", usTime);
                    last_action_time = usTime;
                    exit(1);
                }
                if (serDat==true) {
                    if ( dataEmpty(aktiveChannel) ) {                    
                        if (stdebug) printf("Send done \n");
                        serDat1570 = true; // return to listen 
                        serClk1570 = false; 
                        updateDataline();
                        talkState = TRANSMIT_IDLE;
                    } else {
                        if (stdebug) printf("Send next \n");
                        talkState = TRANSMIT_READY_FOR_DATA_2;
                    }
                }
                break;
#if 0
            case TRANSMIT_RETURN_LISTEN:
                if (serDat==true) {
                    printf("Ak retrun to device\n");
                    exit(1);
                }
                break;
#endif                 
            default:
                printf("[%llu us] Unbekannter Zustand: Zurueck zu DEVICE_IDLE.\n", usTime);
                talkState = TRANSMIT_IDLE;
                break;
        }

    } 
    
    if ((deviceState == DEVICE_LISTEN) && (serAtn==false)) {

        if (check_timeout(last_action_time, 1000)) {  // Timeout 1000us // in device mode we lock for timeouts
            if (transmitState!=TRANSMIT_WAIT_FOR_DATA_1)  printf("[%llu us] empfang timeout -> go idle\n", usTime);
            last_action_time = usTime;
            transmitState = TRANSMIT_IDLE;
            // dbgWait=5000;
        }

        switch (transmitState) {
            case TRANSMIT_IDLE:  //Step 0
                // Step Zero: Beide Leitungen werden gehalten
                if (serClk==true)  { // we will wait fo ridle
                    eoi_detected = false;
                    transmitState = TRANSMIT_WAIT_FOR_DATA_1;
                   // printf("[%llu us] TRANSMIT_IDLE .\n", usTime);
                }
                serClk1570 = false;
                serDat1570 = true;
                updateDataline();
                break;

            case TRANSMIT_WAIT_FOR_DATA_1: // step 1
                if (serClk==false) {  // Step 1: Talker signalisiert "Ready to Send"
                    if (stdebug) printf("[%llu us] Clock auf FALSE. go DEVICE_WAIT_FOR_COMMAND2.\n", usTime);
                    serDat1570 = false;
                    updateDataline();
                    last_action_time = usTime;
                    transmitState = TRANSMIT_WAIT_FOR_DATA_2;            
                }
                break;

            case TRANSMIT_WAIT_FOR_DATA_2: // step 2
                if (serClk==true) {  // Step 2: Talker signalisiert "Ready to Send"
                    if (stdebug) printf("[%llu us] Clock auf True. Empfangen eines Bytes beginnt.\n", usTime);
                    bit_index = 0;
                    received_byte = 0;
                    waitEdge = false;
                    serDat1570 = false;
                    updateDataline();
                    last_action_time = usTime;
                    transmitState = TRANSMIT_RECEIVING;
                }
                
                if (check_timeout(last_action_time, 200)) {  // Timeout 200us
                    if (stdebug) printf("[%llu us] Timeout beim Warten auf Clock.Ab zu DEVICE_EOI.\n", usTime);
                    serDat1570 = true;
                    updateDataline();
                    last_action_time = usTime;
                    transmitState = TRANSMIT_EOI;
                }
                
                break;

            case TRANSMIT_RECEIVING:                        
                if (serClk) {
                    if (bit_index==8) {
                        storeData(aktiveChannel,received_byte);
                        if (deviceState==DEVICE_LISTEN) {
                            if (stdebug) printf("[%llu us] Byte empfangen: 0x%02X    command = 0x%02X\n", usTime, received_byte,received_command);
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
                        if (stdebug) printf("[%llu us] Empfangenes Bit %d: %d\n", usTime, bit_index, serDat ? 1 : 0);
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
                        if (stdebug) printf("[%llu us] ACK gesendet -> go idle\n", usTime);
                    } else {
//                        transmitState = TRANSMIT_WAIT_FOR_DATA_1;
                        transmitState = TRANSMIT_IDLE;
                        if (stdebug) printf("[%llu us] ACK gesendet -> wait next byte\n", usTime);
                    }
                    last_action_time = usTime;
                }
                break;

            case TRANSMIT_EOI:
                if (check_timeout(last_action_time, 70)) {  // 60 µs EOI halten
                    if (stdebug) printf("[%llu us] EOI-ACK abgeschlossen. Wechsel zu DEVICE_RECEIVING.\n", usTime);
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
