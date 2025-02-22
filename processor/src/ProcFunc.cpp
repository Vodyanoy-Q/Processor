#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys\stat.h>

#include "../inc/Stack.h"
#include "../inc/Proc.h"

int GetCode(SPU * spu)
{
    int error = 1;
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    _FOPEN(spu->file, spu->file_name, "rb");

    spu->size = 0;
    _CHECK_ERROR(GetFileSize(spu->file, &(spu->size)));

    spu->code = (double*)calloc(spu->size, sizeof(double));
    MY_ASSERT(spu->code, ADDRESS_ERROR);

    if (fread(spu->code, sizeof(double), spu->size, spu->file) != spu->size)
    {
        printf("ERROR TO READ INPUT FILE\n"
               "FILE NAME: %s\n", spu->file_name);

        return ERROR_READ_FROM_FILE;
    }

    _FCLOSE(spu->file);

    return NO_ERROR;
}

int GetFileSize(FILE * file, size_t * size)
{
    struct stat st;

    if (fstat(fileno(file), &st) == 0)
    {
        *size = st.st_size / sizeof(double);
        return NO_ERROR;
    }

    return GET_SIZE_ERROR;
}

int SPU_Ctor(SPU * spu, char * file_name)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    spu->file = NULL;
    spu->file_name = file_name;

    spu->ip   = 0;
    spu->size = 0;

    spu->code = NULL;

    spu->registers = (double*)calloc(4, sizeof(double));
    MY_ASSERT(spu->registers, ADDRESS_ERROR);

    spu->ram = (double*)calloc(RAM_SIZE, sizeof(double));
    MY_ASSERT(spu->ram, ADDRESS_ERROR);

    spu->stk =  NULL;
    StackCtor(&(spu->stk), 4);
    MY_ASSERT(spu->stk, ADDRESS_ERROR);

    spu->recursion_stk = NULL;
    StackCtor(&(spu->recursion_stk), 4);
    MY_ASSERT(spu->recursion_stk, ADDRESS_ERROR);

    return NO_ERROR;
}

int SPU_Dtor(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    spu->file = NULL;
    spu->file_name = NULL;

    spu->ip = 0;
    spu->size = 0;

    StackDtor(&spu->stk);
    StackDtor(&spu->recursion_stk);

    free(spu->code);
    free(spu->registers);
    free(spu->ram);
    free(spu->stk);
    free(spu->recursion_stk);

    spu->code = NULL;
    spu->registers = NULL;
    spu->ram = NULL;
    spu->stk = NULL;
    spu->recursion_stk = NULL;

    return NO_ERROR;
}

int DoCode(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    CMD cmds[cmd_count] = {{DoPUSH, HAVE_ARG}, {DoADD,   NO_ARG}, {DoSUB,   NO_ARG}, {DoMUL,    NO_ARG}, {DoDIV,  NO_ARG},
                           {DoOUT,    NO_ARG}, {DoHLT,   NO_ARG}, {DoJMP, HAVE_ARG}, {DoJB,   HAVE_ARG}, {DoJA, HAVE_ARG},
                           {DoJE,   HAVE_ARG}, {DoIN,    NO_ARG}, {DoPOP, HAVE_ARG}, {DoCALL, HAVE_ARG}, {DoRET,  NO_ARG},
                           {DoSQRT,   NO_ARG}, {DoJAE, HAVE_ARG}, {DoJBE, HAVE_ARG}, {DoJNE,  HAVE_ARG}, {DoFREE, NO_ARG},
                           {DoWRAM,   NO_ARG} };
    int cmd = 0;
    int error = 0;

    for (spu->ip = 0; spu->ip < spu->size && spu->hlt_status == 0; spu->ip++)
    {
        cmd = (int)(spu->code[spu->ip]) & 0x1F;

        spu->ip += cmds[cmd - 1].get_arg;
        error = cmds[cmd - 1].func(spu);

        if (error != 0)
        {
            printf(RED "ERROR: %d\n" RESET_COLOR, error);
            printf(RED "CMD CODE: %d\n" RESET_COLOR, cmd);
            printf(RED "IP: %d\n" RESET_COLOR, spu->ip - cmds[cmd - 1].get_arg);

            ProcDump(spu);
            SPU_Dtor(spu);
            exit (error);
        }
        //ProcDump(spu);
        //getchar();
    }

    return NO_ERROR;
}
int DoFREE(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    for (int i = 0; i < RAM_SIZE; i++)
    {
        spu->ram[i] = 0;
    }

    return NO_ERROR;
}

int DoWRAM(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    for (int i = 0; i < RAM_SIZE; i++)
    {
        if (spu->ram[i] == 0)
        {
            printf(" ");
        }
        else if (spu->ram[i] == 1)
        {
            printf("*");
        }
        else if (spu->ram[i] == 2)
        {
            printf("\n");
        }
        else if (spu->ram[i] == 3)
        {
            return NO_ERROR;
        }
    }

    return NO_ERROR;
}

int DoCALL(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = NULL;
    _GET_ARG;

    StackPush(spu->recursion_stk, spu->ip + 1);

    spu->ip = (int)(*arg) - 1;

    return NO_ERROR;
}

int DoRET(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    StackType var = 0;
    _STACK_POP(spu->recursion_stk, &var);

    spu->ip = var - 1;

    return NO_ERROR;
}

int GetArg(SPU * spu, double ** arg)
{
    switch ((int)(spu->code[spu->ip - 1]) & 0xE0)
    {
        case NUM:
        {
            *arg = &spu->code[spu->ip];

            break;
        }
        case REG:
        {
            *arg = &spu->registers[(int)(spu->code[spu->ip]) - 1];

            break;
        }
        case RAM + NUM:
        {
            *arg = &spu->ram[(int)(spu->code[spu->ip])];

            break;
        }
        case RAM + REG:
        {
            *arg = &spu->ram[(int)(spu->registers[(int)(spu->code[spu->ip]) - 1])];

            break;
        }
        default:
        {
            printf(RED "ERROR PUSH ARG\n" RESET_COLOR
                   YELLOW "ip = %d\n"
                   "code[%d] = %lg\n" RESET_COLOR, spu->ip, spu->ip, spu->code[spu->ip]);
            return ERROR_ARG;
        }
    }

    return NO_ERROR;
}

int DoPUSH (SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = NULL;
    _GET_ARG;

    StackPush(spu->stk, *arg);

    return NO_ERROR;
}

int DoPOP (SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    if (((int)(spu->code[spu->ip - 2]) & 0xE0) == NUM)
    {
        return NO_ERROR;
    }

    StackType var = 0;

    double * arg = 0;
    _GET_ARG;

    _STACK_POP(spu->stk, &var);

    *arg = var;

    return NO_ERROR;
}

int DoADD(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    StackPush(spu->stk, a + b);

    return NO_ERROR;
}

int DoSQRT(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackPush(spu->stk, sqrt(a));

    return NO_ERROR;
}

int DoSUB(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    StackPush(spu->stk, b - a);

    return NO_ERROR;
}

int DoMUL(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    StackPush(spu->stk, a * b);

    return NO_ERROR;
}

int DoDIV(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    StackPush(spu->stk, b / a);

    return NO_ERROR;
}

int DoOUT(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    printf(YELLOW "%lg\n" RESET_COLOR, a);

    return NO_ERROR;
}

int DoHLT(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    spu->hlt_status = 1;

    return NO_ERROR;
}

int DoJMP(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = 0;
    _GET_ARG;

    spu->ip = (int)(*arg) - 1;

    return NO_ERROR;
}

int DoJA(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = 0;
    _GET_ARG;

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    if (b > a)
    {
        spu->ip = (int)(*arg) - 1;
    }

    return NO_ERROR;
}

int DoJB(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = 0;
    _GET_ARG;

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    if (b < a)
    {
        spu->ip = (int)(*arg) - 1;
    }

    return NO_ERROR;
}

int DoJE(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = 0;
    _GET_ARG;

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    if (b == a)
    {
        spu->ip = (int)(*arg) - 1;
    }

    return NO_ERROR;
}

int DoJAE(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = 0;
    _GET_ARG;

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    if (b >= a)
    {
        spu->ip = (int)(*arg) - 1;
    }

    return NO_ERROR;
}

int DoJBE(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = 0;
    _GET_ARG;

    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    if (b <= a)
    {
        spu->ip = (int)(*arg) - 1;
    }

    return NO_ERROR;
}

int DoJNE(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    double * arg = 0;
    _GET_ARG;


    StackType a = 0;
    _STACK_POP(spu->stk, &a);

    StackType b = 0;
    _STACK_POP(spu->stk, &b);

    if (b != a)
    {
        spu->ip = (int)(*arg) - 1;
    }

    return NO_ERROR;
}

int DoIN(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    YELLOW_TEXT("Enter:\n");

    StackType var = 0;

    scanf("%lg", &var);

    StackPush(spu->stk, var);

    return NO_ERROR;
}

int ProcDump(SPU * spu)
{
    MY_ASSERT(spu, SPU_ADDRESS_ERROR);

    printf(YELLOW "\n=======================================================================\n" RESET_COLOR);

    printf("%sip%s%s:%s   ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    for (unsigned int i = 0; i < 11 && spu->ip + i < spu->size; i++)
    {
        if(i + spu->ip == spu->ip)
        {
            printf(RED "%5d " RESET_COLOR, i + spu->ip);
            continue;
        }
        printf(CYAN "%5d " RESET_COLOR, i + spu->ip);
    }

    printf("\n%scode%s%s:%s ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    for (unsigned int i = 0; i < 11 && spu->ip + i < spu->size; i++)
    {
        if(i + spu->ip == spu->ip)
        {
            printf(RED "%5lg " RESET_COLOR, spu->code[i + spu->ip]);
            continue;
        }
        printf(CYAN "%5lg " RESET_COLOR, spu->code[i + spu->ip]);
    }
    printf("\n");

    printf(MAGN "~~~~~~~~~~^" RESET_COLOR);

    printf("\n%sstk%s%s:%s  ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    DataDump(spu->stk);

    printf("\n%srec_stk%s%s:%s  ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    DataDump(spu->recursion_stk);

    printf(BLUE "\nHLT Status: %d\n" RESET_COLOR, spu->hlt_status);

    printf("%sregs%s:%s%s ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    for (int i = 0; i < 4; i++)
    {
        printf(CYAN "%5lg " RESET_COLOR, spu->registers[i]);
    }

    printf("\n%sram%s:%s%s  ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    for (int i = 0; i < RAM_SIZE; i++)
    {
        printf(CYAN "%5lg " RESET_COLOR, spu->ram[i]);
        if (i % 10 == 9)
        {
            printf("\n      ");
        }
    }

    printf(YELLOW "\n=======================================================================\n" RESET_COLOR);

    return NO_ERROR;
}
