#ifndef _PROC_H_
#define _PROC_H_

#include "commands.h"
#include "colors.h"
#include "ASSERT.h"
#include "ERRORS.h"

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

enum SIZE
{
    RAM_SIZE = 100,
};


void GetCode(SPU * spu);
size_t GetFileSize(FILE * file);
void SPU_Ctor(SPU * spu, char * file_name);
void SPU_Dtor(SPU * spu);
void ProcDump(SPU * spu);
void DoCode(SPU * spu);
void DoPUSH (SPU * spu);
void DoADD(SPU * spu);
void DoSUB(SPU * spu);
void DoMUL(SPU * spu);
void DoDIV(SPU * spu);
void DoOUT(SPU * spu);
void DoHLT(SPU * spu);
void DoIN(SPU * spu);
void DoPOP (SPU * spu);
void DoJMP(SPU * spu);
void DoJB(SPU * spu);
void DoJA(SPU * spu);
void DoJE(SPU * spu);
void DoJBE(SPU * spu);
void DoJAE(SPU * spu);
void DoJNE(SPU * spu);
void DoSQRT(SPU * spu);
void DoCALL(SPU * spu);
void DoRET(SPU * spu);
void DoWRAM(SPU * spu);
void DoFREE(SPU * spu);

#endif // _STAAAK_H_
  //012345678910
//_____*****_____
//___**     **
//__*         *
//_*           *
//_*           *
//*             *
//*             *
//*             *
//*             *
//*             *
// *           *
// *           *
//  *         *
//   **     **
//     *****
