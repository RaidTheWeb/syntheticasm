#include <ctype.h>

#include "assembler.h"
#include "vm.h"

Assembler assembler;

#define SIZE 20

typedef struct {
    uint16_t value;
    const char* key;
} DataItem;

DataItem* labelArray[SIZE];
DataItem* nullItem;

uint32_t hashString(const char* str) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < strlen(str); i++) {
        hash ^= (uint8_t)str[i];
        hash *= 16777619;
    }
    return (int)hash;
}

static int hashCode(const char* str) {
    return hashString(str) % SIZE;
}

DataItem* searchKey(const char* key, DataItem* hashArray[]) {
    int hashIndex = hashCode(key);
    if(hashArray[hashIndex] != NULL)
        return hashArray[hashIndex];

    return NULL;
}

void insertKey(const char* key, uint16_t data, DataItem* hashArray[]) {
    DataItem* item = (DataItem*)malloc(sizeof(DataItem));
    item->value = data;
    item->key = key;

    int hashIndex = hashCode(key);

    hashArray[hashIndex] = item;
}

DataItem* deleteKey(DataItem* item, DataItem* hashArray[]) {
    const char* key = item->key;

    int hashIndex = hashCode(key);

    hashArray[hashIndex] = nullItem;

    return NULL;
}

static void emitByte(uint8_t byte) {
    if(assembler.capacity < assembler.count + 1) {
        int oldCapacity = assembler.capacity;
        assembler.capacity = GROW_CAPACITY(oldCapacity);
        assembler.buffer = GROW_BUFFER(assembler);
    }

    assembler.buffer[assembler.count] = byte;
    assembler.count++;
    assembler.bytesWritten++;
}

static void emitByte16(uint16_t bytes) {
    uint8_t msb = bytes >> 8;
    uint8_t lsb = bytes;
    emitByte(msb);
    emitByte(lsb);
}

static void writeBuffer(FILE* file) {
    fwrite(assembler.buffer, sizeof(uint8_t), assembler.count, file);
}

void trim(char * s) {
    char* p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(*p && isspace(*p)) ++p, --l;

    memmove(s, p, l + 1);
}

char** split(char* str, char* delim) {
    char** res = NULL;
    char* p = strtok(str, delim);
    int nspaces = 0, i;

    while(p) {
        res = realloc(res, sizeof(char*) * ++nspaces);

        if(res == NULL) {
            fprintf(stderr, "out of memory.\n");
            exit(1);
        }

        res[nspaces-1] = p;

        p = strtok(NULL, delim);
    }

    res = realloc(res, sizeof(char*) * (nspaces+1));
    res[nspaces] = 0;

    return res;
}

static uint8_t getRegisterHex(const char* reg) {
    switch(hashString(reg)) {
        case HR_R0: return r0;
        case HR_R1: return r1;
        case HR_R2: return r2;
        case HR_R3: return r3;
        default: {
            fprintf(stderr, "invalid register `%s`.\n", reg);
            exit(1);
        }
    }
    return 0x00;
}

static void matchArgs(char** args, int expected) {
    size_t num = (sizeof(args) / sizeof(args[0])) + 1;
    if(num < expected) {
        fprintf(stderr, "argument mismatch for instruction `%s`.\n", args[0]);
        exit(1);
    }
}

static bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

static bool match(const char* line, char chr, int index) {
    return line[index] == chr;
}

static bool isAlpha(char c) {
    return ((c >= 'a' &&
            c <= 'z') ||
            (c >= 'A' &&
            c <= 'Z'));
}

char* matchLabel(char* line) {
    int length = (int)strlen(line);
    if(length < 2) return NULL;
    char name[length];
    int k = 0;
    for(int i = 0; i < length; i++) {
        if(isAlpha(line[i]) != 1) break;
        name[i] = line[i];
        k++;
    }
    if(line[k] == ':') {
        name[k + 1] = '\0';
        return strdup(name);
    }
    return NULL;
}

static bool isAplhaStr(char* line) {
    for(int i = 0; i < strlen(line); i++) {
        if(isAlpha(line[i]) != 1) return false;
    }
    return true;
}

void assemble(FILE* file) {
    assembler.bytesWritten = 0x00;
    assembler.count = 0;
    assembler.capacity = 8;
    assembler.buffer = malloc(sizeof(uint8_t) * assembler.capacity);

    char* line = NULL;
    char** splitline = NULL;
    size_t len = 0;
    ssize_t read;

    while((read = getline(&line, &len, file)) != -1) {
        line[strcspn(line, "\n")] = 0; // remove trailing newline
        trim(line);
        splitline = split(line, ", ");

        if(strcmp(line, "") == 0) continue; // empty line
        if(startsWith(";", line)) continue; // single line comment
        char* label = matchLabel(splitline[0]);
        if(label != NULL) {
            insertKey(label, assembler.bytesWritten, labelArray);
            continue; // label
        }

        switch(hashString(splitline[0])) {
            case H_HALT: {
                emitByte(OP_HALT);
                break;
            }
            case H_MOV: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_MOV);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_PRINTC: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_PRINTC);
                emitByte(reg);
                break;
            }
            case H_PRINTCS: {
                matchArgs(splitline, 1);
                size_t index = strlen(splitline[0]) + 1;
                char lastChar;
                if(line[index] == '\"') index++;
                emitByte(OP_PRINTCS);
                for(;;) {
                    lastChar = line[index++];
                    if(lastChar == '\n') {
                        fprintf(stderr, "unterminated string.\n");
                        exit(1);
                    }
                    if(lastChar == '\"') break;
                    if(lastChar == 0x00) emitByte((uint16_t)0x20);
                    else emitByte((uint8_t)lastChar);
                }
                emitByte(0x00); // null terminate string
                break;
            }
            case H_PRINTI: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_PRINTI);
                emitByte(reg);
                break;
            }
            case H_PRINTH: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_PRINTH);
                emitByte(reg);
                break;
            }
            case H_SETR: {
                matchArgs(splitline, 2);
                uint8_t reg = getRegisterHex(splitline[1]);
                uint16_t value = (uint16_t)strtol(splitline[2], NULL, 0);
                emitByte(OP_SETR);
                emitByte(reg);
                emitByte16(value);
                break;
            }
            case H_INC: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_INC);
                emitByte(reg);
                break;
            }
            case H_DEC: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_DEC);
                emitByte(reg);
                break;
            }
            case H_ADD: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_ADD);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_SUB: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_SUB);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_MUL: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_MUL);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_DIV: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_DIV);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_JMP: {
                matchArgs(splitline, 1);
                uint16_t dest;
                if(!isAplhaStr(splitline[1]))
                    dest = (uint16_t)strtol(splitline[1], NULL, 0);
                else
                    dest = searchKey(splitline[1], labelArray)->value;
                emitByte(OP_JMP);
                emitByte16(dest);
                break;
            }
            case H_JNZ: {
                matchArgs(splitline, 2);
                uint8_t reg = getRegisterHex(splitline[1]);
                uint16_t dest = 0x0000;
                if(isAplhaStr(splitline[2]) != 1)
                    dest = (uint16_t)strtol(splitline[2], NULL, 0);
                else
                    dest = (uint16_t)searchKey(splitline[2], labelArray)->value;
                emitByte(OP_JNZ);
                emitByte(reg);
                emitByte16(dest);
                break;
            }
            case H_JZ: {
                matchArgs(splitline, 2);
                uint8_t reg = getRegisterHex(splitline[1]);
                uint16_t dest;
                if(isAplhaStr(splitline[2]) != 1)
                    dest = (uint16_t)strtol(splitline[2], NULL, 0);
                else
                    dest = (uint16_t)searchKey(splitline[2], labelArray)->value;
                emitByte(OP_JZ);
                emitByte(reg);
                emitByte16(dest);
                break;
            }
            case H_SHL: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_SHL);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_SHR: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_SHR);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_XOR: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_XOR);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_OR: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_OR);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_AND: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_AND);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_POP: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_POP);
                emitByte(reg);
                break;
            }
            case H_PUSH: {
                matchArgs(splitline, 1);
                uint16_t data = (uint16_t)strtol(splitline[1], NULL, 0);
                emitByte(OP_PUSH);
                emitByte16(data);
                break;
            }
            case H_PUSHR: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_PUSHR);
                emitByte(reg);
                break;
            }
            case H_GETIP: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_GETIP);
                emitByte(reg);
                break;
            }
            case H_PEEK: {
                matchArgs(splitline, 1);
                uint8_t reg = getRegisterHex(splitline[1]);
                emitByte(OP_PEEK);
                emitByte(reg);
                break;
            }
            case H_MOD: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_MOD);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_LT: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_LT);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            case H_GT: {
                matchArgs(splitline, 2);
                uint8_t reg1 = getRegisterHex(splitline[1]);
                uint8_t reg2 = getRegisterHex(splitline[2]);
                emitByte(OP_GT);
                emitByte(reg1);
                emitByte(reg2);
                break;
            }
            default: {
                fprintf(stderr, "invalid instruction `%s`.\n", splitline[0]);
                exit(1);
            }
        }
    }

    fclose(file);
    if(line)
        free(line);

    FILE* out = fopen("a.out", "wb");
    writeBuffer(out);
}