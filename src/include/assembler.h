#pragma once

#include <stdio.h>

#include "common.h"
#include "opcodes.h"

typedef struct {
    uint8_t bytesWritten;
    int count;
    int capacity;
    uint8_t* buffer;  
} Assembler;

#define GROW_BUFFER(assembler) assembler.buffer = realloc(assembler.buffer, ++assembler.capacity)

#define GROW_CAPACITY(capacity) (++capacity)

void assemble(FILE* file);