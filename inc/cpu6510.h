#include <stdint.h>


#include "opcodes.h"
#include "functions.h"

#define MEMORY_SIZE 65536  // 64KB Speicher für die 6510 CPU

#define STACK_BASE 0x100

#define BASIC_ROM_ADDR 0Xa000
#define BASIC_ROM_LEN  0X2000
#define CORE_ROM_ADDR  0XE000
#define CORE_ROM_LEN   0X2000
#define CHAR_ROM_ADDR  0XD000
#define CHAR_ROM_LEN   0X1000


#define FLAG_CARRY     0X01
#define FLAG_ZERO      0X02
#define FLAG_INTERRUPT 0X04
#define FLAG_DECIMAL   0X08
#define FLAG_BREAK     0X10
#define FLAG_UNUSED    0X20
#define FLAG_OVERFLOW  0X40
#define FLAG_NEGATIVE  0X80


// CPU-Register
typedef struct {
    uint8_t A;       // Akkumulator
    uint8_t X;       // cpu.X-Register
    uint8_t Y;       // Y-Register
    uint8_t SP;      // Stack Pointer
    uint16_t PC;     // Program Counter
    uint8_t SR;      // Status Register (Flag Register)
} CPU;

// Deklaration von Funktionszeigern für jeden OPCODE
typedef void (*OPCODE_func)(void);


extern uint8_t memory[MEMORY_SIZE];  // Speicher
extern uint8_t rom[MEMORY_SIZE];  // Speicher
extern CPU cpu;
extern uint32_t clkCount;
