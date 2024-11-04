#ifndef TRACE_H
#define TRACE_H

#include <stdint.h>

#define ACTION_OPCODE_0     1 
#define ACTION_OPCODE_1     2 
#define ACTION_OPCODE_2     3 
#define ACTION_RD_MEM       4
#define ACTION_WR_MEM       5
#define ACTION_START_IRQ    6
#define ACTION_END_IRQ      7
#define ACTION_RD_VIC       8
#define ACTION_WR_VIC       9
#define ACTION_RD_IO       10
#define ACTION_WR_IO       11

#define MEM_ROM    1
#define MEM_RAM    2
#define MEM_IO     3
#define MEM_CHAR   4
#define MEM_COLOR  5
#define MEM_IGNORE 6



#define MAXTRACE 1000000

typedef struct  {
    uint32_t cycle;
    int32_t irq;
    int32_t inIrq;
    uint16_t action;
    uint16_t opcode;
    uint16_t data1;
    uint16_t data2;
    uint16_t addr;
    uint16_t value;
    uint16_t vicMemCtrl;
    uint16_t vicCtrl1;
    uint16_t vicCtrl2;
    uint16_t raster;
    uint16_t cpuPort1;
    uint16_t cpuPort2;
    uint16_t seg;
    uint16_t a;
    uint16_t x;
    uint16_t y;
    uint16_t sp;
    uint16_t pc;
    uint16_t sr;    
} trace_t;

extern trace_t trace[MAXTRACE];

extern void updateTraceStopIRQ(void);
extern void updateTraceStartIRQ(void);
extern void updateTraceRDVIC( uint16_t addr,uint8_t value);
extern void updateTraceWRVIC( uint16_t addr,uint8_t value);
extern void updateTraceWRMem( uint16_t addr,uint8_t value,uint8_t seg);
extern void updateTraceRDMem( uint16_t addr,uint8_t value,uint8_t seg);
extern void updateTraceRDIO( uint16_t addr,uint8_t value);
extern void updateTraceWRIO( uint16_t addr,uint8_t value);
extern void updateTraceOpcode( uint8_t opcode , uint8_t data1 , uint8_t data2);
extern void writeTrace(void);

#endif