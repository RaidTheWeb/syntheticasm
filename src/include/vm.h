#pragma once

#include "common.h"
#include "opcodes.h"

#define NUM_REGS 15
#define STACK_MAX 256

typedef struct {
    uint8_t* source;
    uint16_t ip;
    uint8_t secip;
    uint16_t regs[NUM_REGS];
    uint16_t stack[STACK_MAX];
    uint16_t* stackTop; 
} VM;

typedef enum {
    r0 = 0x00,
    r1 = 0x01,
    r2 = 0x02,
    r3 = 0x03,
    r4 = 0x04,
    r5 = 0x05,
    r6 = 0x06,
    r7 = 0x07,
    r8 = 0x08,
    r9 = 0x09,
    r10 = 0x0A,
    ax = 0x0B,
    bx = 0x0C,
    cx = 0x0D,
    dx = 0x0E,
} Registers;

#define VALID_REGISTER(reg) \
    (reg <= (NUM_REGS - 1))

void initVM();
void freeVM();
void run(uint8_t* source);