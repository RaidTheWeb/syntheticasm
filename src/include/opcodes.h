#pragma once

typedef enum {
    OP_HALT         = 0x01,                     // halt     -       halt CPU
    OP_MOV          = 0x02,                     // mov      -       move value into register
    OP_PRINTC       = 0x03,                     // printc   -       print register value as character
    OP_PRINTCS      = 0x04,                     // printcs  -       print character string (terminated by 00)
    OP_PRINTI       = 0x05,                     // printi   -       print register value as integer
    OP_PRINTH       = 0x06,                     // printh   -       print register value as hexadecimal (base16)
    OP_SETR         = 0x07,                     // setr     -       set register value to byte
    OP_INC          = 0x08,                     // inc      -       increment register value
    OP_DEC          = 0x09,                     // dec      -       decrement register value
    OP_ADD          = 0x0A,                     // add      -       add register values together and store in dest
    OP_SUB          = 0x0B,                     // sub      -       subtract register values together and store in dest
    OP_MUL          = 0x0C,                     // mul      -       multiply register values together and store in dest
    OP_DIV          = 0x0D,                     // div      -       divide register values together and store in dest
    OP_JMP          = 0x0E,                     // jmp      -       jump past code in program
    OP_JNZ          = 0x0F,                     // jnz      -       jump past code in program if register is a non-zero value
    OP_JZ           = 0x10,                     // jz       -       jump past code in program if register value is zero
    OP_SHL          = 0x11,                     // shl      -       shift register value left by register value
    OP_SHR          = 0x12,                     // shr      -       shift register value right by register value
    OP_XOR          = 0x13,                     // xor      -       xor register value by register value and store in dest
    OP_OR           = 0x14,                     // or       -       binary or register value by register value and store in dest
    OP_AND          = 0x15,                     // and      -       binary and register value by register value and store in dest
    OP_POP          = 0x16,                     // pop      -       pop value from stack into register
    OP_PUSH         = 0x17,                     // push     -       push value to stack
    OP_PUSHR        = 0x18,                     // pushr    -       push register value to stack
    OP_GETIP        = 0x19,                     // getip    -       get instruction pointer and store in register
    OP_PEEK         = 0x1A,                     // peek     -       peek at value from stack and place in register
    OP_MOD          = 0x1B,                     // mod      -       modulate register values together and store in dest
    OP_LT           = 0x1C,                     // lt       -       conditional less than and store result in dest
    OP_GT           = 0x1D,                     // gt       -       conditional greater than and store result in dest
    OP_RET          = 0x1E,                     // ret      -       return from procedure (restore ip from stack)
    OP_CALL         = 0x1F,                     // call     -       call a procedure (place ip on stack)
    OP_PRINTIS      = 0x20,                     // printis  -       print integer from stack
    OP_ADDS         = 0x21,                     // adds     -       add two values from stack and push result to stack
    OP_SUBS         = 0x22,                     // subs     -       subtract two values from stack and push result to stack
    OP_MULS         = 0x23,                     // muls     -       multiply two values from stack and push result to stack
    OP_DIVS         = 0x24,                     // divs     -       divide two values from stack and push result to stack
    OP_LTS          = 0x25,                     // lts      -       conditional less than and push result to stack
    OP_GTS          = 0x26,                     // gts      -       conditional greater than and push result to stack
} Opcode;

typedef enum {
    H_HALT              = 0xe8bee4fa,       // halt
    H_MOV               = 0xd19e5c79,       // mov
    H_PRINTC            = 0xe46faff1,       // printc
    H_PRINTCS           = 0x1dd149a6,       // printcs
    H_PRINTI            = 0xda6fa033,       // printi
    H_PRINTH            = 0xd96f9ea0,       // printh
    H_SETR              = 0x6070b6e3,       // setr
    H_INC               = 0xa8e99c47,       // inc
    H_DEC               = 0xc25979d3,       // dec
    H_ADD               = 0x3b391274,       // add
    H_SUB               = 0xdc4e3915,       // sub
    H_MUL               = 0xeb84ed81,       // mul
    H_DIV               = 0xe562ab48,       // div
    H_JMP               = 0xc3cf89c0,       // jmp
    H_JNZ               = 0xcfd65869,       // jnz
    H_JZ                = 0x573f5905,       // jz
    H_SHL               = 0xe02debb6,       // shl
    H_SHR               = 0xda2de244,       // shr
    H_XOR               = 0xcc6bdb7e,       // xor
    H_OR                = 0x5d342984,       // or
    H_AND               = 0x0f29c2a6,       // and
    H_POP               = 0x51335fd0,       // pop
    H_PUSH              = 0x876fffdd,       // push
    H_PUSHR             = 0xe44f807d,       // pushr
    H_GETIP             = 0x5360a25e,       // getip
    H_PEEK              = 0xaf15011e,       // peek
    H_MOD               = 0xdf9e7283,       // mod
    H_LT                = 0x5d31eaed,       // lt
    H_GT                = 0x4b208576,       // gt
    H_RET               = 0x30f467ac,       // ret
    H_CALL              = 0xb3f184a9,       // call
    H_PRINTIS           = 0x1db944c0,       // printis
    H_ADDS              = 0x41d76105,       // adds
    H_SUBS              = 0x35245b92,       // subs
    H_MULS              = 0xb44293f6,       // muls
    H_DIVS              = 0x55538de1,       // divs
    H_LTS               = 0x539456ba,       // lts
    H_GTS               = 0x493166df,       // gts
} HashedOpcode;

typedef enum {
    HR_R0               = 0x0d53ff3f,               // r0
    HR_R1               = 0x0c53fdac,               // r1
    HR_R2               = 0x0f540265,               // r2
    HR_R3               = 0x0e5400d2,               // r3
    HR_R4               = 0x0953f8f3,               // r4
    HR_R5               = 0x0853f760,               // r5
    HR_R6               = 0x0b53fc19,               // r6
    HR_R7               = 0x0a53fa86,               // r7
    HR_R8               = 0x15540bd7,               // r8
    HR_R9               = 0x14540a44,               // r9
    HR_R10              = 0x04383c94,               // r10
    HR_AX               = 0x53250f3c,               // ax
    HR_BX               = 0x552bce27,               // bx
    HR_CX               = 0x4f29861e,               // cx
    HR_DX               = 0x511cbd51,               // dx
} HashedRegister;