#include <stdio.h>

#include "debug.h"
#include "vm.h"

VM vm;

static uint8_t READ_BYTE() {
    return vm.source[vm.ip++];
}

static uint16_t READ_BYTE16() {
    return (uint16_t)((READ_BYTE() << 8) | READ_BYTE());
}

void initVM() {
    vm.source = NULL;
    vm.ip = 0;
    vm.stackTop = vm.stack;
}

void freeVM() {
    vm.source = NULL;
    vm.ip = 0;
}

static void push(uint16_t value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

static uint16_t pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

static void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if(newSize == 0) {
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, newSize);
    if (result == NULL)
        exit(1);
    return result;
}

void run(uint8_t* source) {
    vm.source = source;
    //vm.ip = vm.source;
    vm.ip = 0;

    for (;;) {
#ifdef DEBUG_TRACE_EXEC
        printf("\n");
        disassembleInstruction(vm.source, vm.ip);
#endif

        uint8_t instruction;
        switch(instruction = READ_BYTE()) {
            case OP_HALT:
                return;
            case OP_MOV: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] = vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_PRINTC: {
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(src))
                    printf("%c", vm.regs[src]);
                else {
                    fprintf(stderr, "invalid register %02x\n", src);
                    exit(1);
                }
                break;
            }
            case OP_PRINTCS: {
                uint8_t lastchar;
                for(;;) {
                    lastchar = READ_BYTE();
                    if(lastchar == 0x00) break;
                    printf("%c", (char)lastchar);
                }
                break;
            }
            case OP_PRINTI: {
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(src)) {
                    printf("%d", vm.regs[src]);
                } else {
                    fprintf(stderr, "invalid register %02x\n", src);
                    exit(1);
                }
                break;
            }
            case OP_PRINTH: {
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(src)) {
                    printf("%04x", vm.regs[src]);
                } else {
                    fprintf(stderr, "invalid register %02x\n", src);
                    exit(1);
                }
                break;
            }
            case OP_SETR: {
                uint8_t dest = READ_BYTE();
                uint16_t data = READ_BYTE16();
                if(VALID_REGISTER(dest)) {
                    vm.regs[dest] = data;
                } else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_INC: {
                uint8_t dest = READ_BYTE();
                if(VALID_REGISTER(dest))
                    vm.regs[dest]++;
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_DEC: {
                uint8_t dest = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(vm.regs[dest] > 0x0000)
                        vm.regs[dest]--;
                    else {
                        fprintf(stderr, "attempted negative decrementation of register\n");
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_ADD: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] += vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_SUB: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        if(vm.regs[dest] > vm.regs[src])
                            vm.regs[dest] -= vm.regs[src];
                        else {
                            fprintf(stderr, "attempted negative decrementation of register\n");
                            exit(1);
                        }
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_MUL: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] *= vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }

            case OP_DIV: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        if(vm.regs[dest] != 0x00)
                            if(vm.regs[src] != 0x00)
                                vm.regs[dest] /= vm.regs[src];
                            else {
                                fprintf(stderr, "attempted division by zero of register");
                                exit(1);
                            }
                        else {
                            fprintf(stderr, "attempted division by zero of register");
                                exit(1);
                        }
                        
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_JMP: {
                uint16_t data = READ_BYTE16();
                vm.ip = data;
                break;
            }
            case OP_JNZ: {
                uint8_t src = READ_BYTE();
                uint16_t data = READ_BYTE16();
                if(VALID_REGISTER(src)) {
                    if(vm.regs[src] > 0x00)
                        vm.ip = data;
                } else {
                    fprintf(stderr, "invalid register %02x\n", src);
                    exit(1);
                }
                break;
            }
            case OP_JZ: {
                uint8_t src = READ_BYTE();
                uint16_t data = READ_BYTE16();
                if(VALID_REGISTER(src)) {
                    if(vm.regs[src] == 0x00)
                        vm.ip = data;
                } else {
                    fprintf(stderr, "invalid register %02x\n", src);
                    exit(1);
                }
                break;
            }
            case OP_SHL: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] <<= vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_SHR: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] >>= vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_XOR: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] ^= vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_OR: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] |= vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_AND: {
                uint8_t dest = READ_BYTE();
                uint16_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] &= vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_POP: {
                uint16_t dest = READ_BYTE();
                if(VALID_REGISTER(dest)) {
                    vm.regs[dest] = pop();
                } else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_PUSH: {
                uint16_t data = READ_BYTE16();
                push(data);
                break;
            }
            case OP_PUSHR: {
                uint8_t reg = READ_BYTE();
                if(VALID_REGISTER(reg)) {
                    push(vm.regs[reg]);
                } else {
                    fprintf(stderr, "invalid register %02x\n", reg);
                    exit(1);
                }
                break;
            }
            case OP_GETIP: {
                uint8_t reg = READ_BYTE();
                if(VALID_REGISTER(reg)) {
                    vm.regs[reg] = (uint16_t)((uint8_t)(0x00 << 8) | (uint8_t)vm.ip);
                } else {
                    fprintf(stderr, "invalid register %02x\n", reg);
                    exit(1);
                }
                break;
            }
            case OP_PEEK: {
                uint8_t reg = READ_BYTE();
                if(VALID_REGISTER(reg)) {
                    vm.regs[reg] = pop();
                } else {
                    fprintf(stderr, "invalid register %02x\n", reg);
                    exit(1);
                }
                break;
            }
            case OP_MOD: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] %= vm.regs[src];
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_LT: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] = vm.regs[dest] < vm.regs[src] ? 1 : 0;
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_GT: {
                uint8_t dest = READ_BYTE();
                uint8_t src = READ_BYTE();
                if(VALID_REGISTER(dest))
                    if(VALID_REGISTER(src))
                        vm.regs[dest] = vm.regs[dest] > vm.regs[src] ? 1 : 0;
                    else {
                        fprintf(stderr, "invalid register %02x\n", src);
                        exit(1);
                    }
                else {
                    fprintf(stderr, "invalid register %02x\n", dest);
                    exit(1);
                }
                break;
            }
            case OP_RET: {
                vm.ip = pop();
                break;
            }
            case OP_CALL: {
                uint16_t dest = READ_BYTE16();
                push(vm.ip);
                vm.ip = dest;
                break;
            }
            default:
                break;
        }
    }
    
}