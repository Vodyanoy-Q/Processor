#ifndef _COMMANDS_H_
#define _COMMANDS_H_

const int cmd_count = 22;

enum CMD_CODE
{
    CMD_DEFAULT = 0,

    PUSH = 1,
    ADD  = 2,
    SUB  = 3,
    MUL  = 4,
    DIV  = 5,
    OUT  = 6,
    HLT  = 7,
    JMP  = 8,
    JB   = 9,
    JA   = 10,
    JE   = 11,
    IN   = 12,
    POP  = 13,
    CALL = 14,
    RET  = 15,
    SQRT = 16,
    JAE  = 17,
    JBE  = 18,
    JNE  = 19,
    FREE = 20,
    WRAM = 21
};

enum ARG
{
    NUM = 32,
    REG = 64,
    RAM = 128,

    NO_ARG = 0,
    HAVE_ARG = 1
};

#endif //_COMMANDS_H_
