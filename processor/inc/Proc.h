#ifndef _PROC_H_
#define _PROC_H_

#include "commands.h"
#include "colors.h"
#include "ASSERT.h"
#include "ERRORS.h"

const int RAM_SIZE = 100;

struct SPU
{
    FILE * file = NULL;
    char * file_name = NULL;

    unsigned int ip = 0;
    int hlt_status = 0;

    unsigned int size = 0;

    double * code = NULL;
    double * registers = NULL;
    double * ram = NULL;

    struct Stack * recursion_stk = NULL;
    struct Stack * stk = NULL;
};

struct CMD
{
    int (*func)(SPU * spu);
    enum ARG get_arg;
};

int GetCode(SPU * spu);
int GetFileSize(FILE * file, size_t * size);
int SPU_Ctor(SPU * spu, char * file_name);
int GetArg(SPU * spu, double ** arg);
int SPU_Dtor(SPU * spu);
int ProcDump(SPU * spu);
int DoCode(SPU * spu);
int DoPUSH (SPU * spu);
int DoADD(SPU * spu);
int DoSUB(SPU * spu);
int DoMUL(SPU * spu);
int DoDIV(SPU * spu);
int DoOUT(SPU * spu);
int DoHLT(SPU * spu);
int DoIN(SPU * spu);
int DoPOP (SPU * spu);
int DoJMP(SPU * spu);
int DoJB(SPU * spu);
int DoJA(SPU * spu);
int DoJE(SPU * spu);
int DoJBE(SPU * spu);
int DoJAE(SPU * spu);
int DoJNE(SPU * spu);
int DoSQRT(SPU * spu);
int DoCALL(SPU * spu);
int DoRET(SPU * spu);
int DoWRAM(SPU * spu);
int DoFREE(SPU * spu);

#endif // _STAAAK_H_
