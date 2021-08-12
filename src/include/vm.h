#pragma once

#include "common.h"
#include "opcodes.h"

#define NUM_REGS 4
#define STACK_MAX 256

typedef struct {
    uint8_t* source;
    uint16_t ip;
    //uint8_t* ip;
    uint8_t secip;
    uint16_t regs[NUM_REGS];
    uint16_t stack[STACK_MAX];
    uint16_t* stackTop; 
} VM;

typedef enum {
    r0 = 0x00,
    r1 = 0x01,
    r2 = 0x02,
    r3 = 0x03
} Registers;

#define VALID_REGISTER(reg) \
    (reg <= (NUM_REGS - 1))

void initVM();
void freeVM();
void run(uint8_t* source);