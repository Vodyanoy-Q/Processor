#ifndef _COMMANDS_H_
#define _COMMANDS_H_

// JAE JBE JNE

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
    LABEL = 17,
    JAE  = 18,
    JBE  = 19,
    JNE  = 20
};

enum ARG
{
    RAM = 128,
    REG = 64,
    NUM = 32
};

#endif //_COMMANDS_H_
