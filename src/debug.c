#include <stdio.h>

#include "debug.h"
#include "opcodes.h"
#include "vm.h"

void disassembleSource(uint8_t* source, const char* name, int length) {
    printf("== %s ==\n", name);

    for(int offset = 0; offset < length; offset++) {
        printf("\n");
        disassembleInstruction(source, offset);
    }
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static char* getRegister(uint8_t reg) {
    if(!VALID_REGISTER(reg)) return "(nil)";
    switch(reg) {
        case r0:    return "r0";
        case r1:    return "r1";
        case r2:    return "r2";
        case r3:    return "r3";
        case r4:    return "r4";
        case r5:    return "r5";
        case r6:    return "r6";
        case r7:    return "r7";
        case r8:    return "r8";
        case r9:    return "r9";
        case r10:   return "r10";
        case ax:    return "ax";
        case bx:    return "bx";
        case cx:    return "cx";
        case dx:    return "dx";
    }
}

static int movInstruction(const char* name, int offset, uint8_t reg1, uint8_t reg2) {
    printf("%s %4s, %s\n", name, getRegister(reg1), getRegister(reg2));
}

static int simplePrintInstruction(const char* name, int offset, uint8_t reg) {
    printf("%s %4s\n", name, getRegister(reg));
}

static int printInstruction(const char* name, int offset, uint8_t* source) {
    printf("%s    \"", name);
    uint8_t lastchar;
    for(;;) {
        lastchar = source[offset++];
        if(lastchar == 0x00) break;
        if(lastchar != 0x0A)
            printf("%c", (char)lastchar);
        else
            printf("\\n");
    }
    printf("\", 0\n");
}

static int setRegisterInstruction(const char* name, int offset, uint8_t reg, uint8_t p1, uint8_t p2) {
    printf("%s %4s, 0x%04x", name, getRegister(reg), (uint16_t)((p1 << 8) | p2));
}

static int simpleRegisterInstruction(const char* name, int offset, uint8_t reg) {
    printf("%s %4s", name, getRegister(reg));
}

static int simpleJumpInstruction(const char* name, int offset, uint8_t p1, uint8_t p2) {
    printf("%s    0x%04x", name, (uint16_t)((p1 << 8) | p2));
}

static int conditionalJumpInstruction(const char* name, int offset, uint8_t reg, uint8_t p1, uint8_t p2) {
    printf("%s %4s, 0x%04x", name, getRegister(reg), (uint16_t)((p1 << 8) | p2));
}

int disassembleInstruction(uint8_t* source, int offset) {
    printf("0x%04x      ", offset);

    uint8_t instruction = source[offset];
    switch(instruction) {
        case OP_HALT: return simpleInstruction("halt", offset);
        case OP_MOV: return movInstruction("mov", offset, source[offset + 1], source[offset + 2]);
        case OP_PRINTC: return simplePrintInstruction("printc", offset, source[offset + 1]);
        case OP_PRINTCS: return printInstruction("printcs", offset, source);
        case OP_PRINTI: return simplePrintInstruction("printi", offset, source[offset + 1]);
        case OP_PRINTH: return simplePrintInstruction("printh", offset, source[offset + 1]);
        case OP_SETR: return setRegisterInstruction("setr", offset, source[offset + 1], source[offset + 2], source[offset + 3]);
        case OP_INC: return simpleRegisterInstruction("inc", offset, source[offset + 1]);
        case OP_DEC: return simpleRegisterInstruction("dec", offset, source[offset + 1]);
        case OP_ADD: return movInstruction("add", offset, source[offset + 1], source[offset + 2]);
        case OP_SUB: return movInstruction("sub", offset, source[offset + 1], source[offset + 2]);
        case OP_MUL: return movInstruction("mul", offset, source[offset + 1], source[offset + 2]);
        case OP_DIV: return movInstruction("div", offset, source[offset + 1], source[offset + 2]);
        case OP_JMP: return simpleJumpInstruction("jmp", offset, source[offset + 1], source[offset + 2]);
        case OP_JNZ: return conditionalJumpInstruction("jnz", offset, source[offset + 1], source[offset + 2], source[offset + 3]);
        case OP_JZ: return conditionalJumpInstruction("jz", offset, source[offset + 1], source[offset + 2], source[offset + 3]);
        case OP_SHL: return movInstruction("shl", offset, source[offset + 1], source[offset + 2]);
        case OP_SHR: return movInstruction("shr", offset, source[offset + 1], source[offset + 2]);
        case OP_XOR: return movInstruction("xor", offset, source[offset + 1], source[offset + 2]);
        case OP_OR: return movInstruction("or", offset, source[offset + 1], source[offset + 2]);
        case OP_AND: return movInstruction("and", offset, source[offset + 1], source[offset + 2]);
        case OP_PUSH: return simpleJumpInstruction("push", offset, source[offset + 1], source[offset + 2]);
        case OP_POP: return simpleRegisterInstruction("pop", offset, source[offset + 1]);
        case OP_PUSHR: return simpleRegisterInstruction("pushr", offset, source[offset + 1]);
        case OP_GETIP: return simpleRegisterInstruction("getip", offset, source[offset + 1]);
        case OP_PEEK: return simpleRegisterInstruction("peek", offset, source[offset + 1]);
        case OP_MOD: return movInstruction("mod", offset, source[offset + 1], source[offset + 2]);
        case OP_LT: return movInstruction("lt", offset, source[offset + 1], source[offset + 2]);
        case OP_GT: return movInstruction("gt", offset, source[offset + 1], source[offset + 2]);
        case OP_RET: return simpleInstruction("ret", offset);
        case OP_CALL: return simpleJumpInstruction("call", offset, source[offset + 1], source[offset + 2]);
        case OP_PRINTIS: return simpleInstruction("printis", offset);
        case OP_ADDS: return simpleInstruction("adds", offset);
        case OP_SUBS: return simpleInstruction("subs", offset);
        case OP_MULS: return simpleInstruction("muls", offset);
        case OP_DIVS: return simpleInstruction("divs", offset);
        case OP_GTS: return simpleInstruction("gts", offset);
        case OP_LTS: return simpleInstruction("lts", offset);
        default:
            printf("unknown operation %02x", instruction);
            return offset + 1;
    }
}