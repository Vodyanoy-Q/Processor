#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys\stat.h>

#include "../inc/Stack.h"
#include "../inc/Proc.h"

void GetCode(SPU * spu)
{
    MY_ASSERT(spu);

    _FOPEN(spu->file, spu->file_name, "rb");

    spu->size = GetFileSize(spu->file) / sizeof(double);

    spu->code = (double*)calloc(spu->size, sizeof(double));
    MY_ASSERT(spu->code);

    if (fread(spu->code, sizeof(double), spu->size, spu->file) != spu->size)
    {
        printf("ERROR TO READ INPUT FILE\n"
               "FILE NAME: %s\n", spu->file_name);
    }

    _FCLOSE(spu->file);
}

size_t GetFileSize(FILE * file)
{
    struct stat st;

    if (fstat(fileno(file), &st) == 0)
    {
        return st.st_size;
    }

    return 0;
}

void SPU_Ctor(SPU * spu, char * file_name)
{
    MY_ASSERT(spu);

    spu->file = NULL;
    spu->file_name = file_name;

    spu->ip   = 0;
    spu->size = 0;

    spu->code = NULL;

    spu->registers = (double*)calloc(4, sizeof(double));
    MY_ASSERT(spu->registers);

    spu->ram = (double*)calloc(RAM_SIZE, sizeof(double));
    MY_ASSERT(spu->ram);

    spu->stk = StackCtor(4);;
    MY_ASSERT(spu->stk);

    spu->recursion_stk = StackCtor(4);;
    MY_ASSERT(spu->recursion_stk);
}

void SPU_Dtor(SPU * spu)
{
    MY_ASSERT(spu);

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
}

void DoCode(SPU * spu)
{
    MY_ASSERT(spu);

    for (spu->ip = 0; spu->ip < spu->size && spu->hlt_status == 0;)
    {
        //ProcDump(spu);

        switch((int)(spu->code[spu->ip]) & 0x1F)
        {
            case PUSH:
            {
                spu->ip++;
                DoPUSH(spu);
                break;
            }
            case ADD:
            {
                DoADD(spu);
                break;
            }
            case SUB:
            {
                DoSUB(spu);
                break;
            }
            case MUL:
            {
                DoMUL(spu);
                break;
            }
            case DIV:
            {
                DoDIV(spu);
                break;
            }
            case OUT:
            {
                DoOUT(spu);
                break;
            }
            case HLT:
            {
                DoHLT(spu);
                break;
            }
            case IN:
            {
                DoIN(spu);
                break;
            }
            case SQRT:
            {
                DoSQRT(spu);
                break;
            }
            case POP:
            {
                spu->ip++;
                DoPOP(spu);
                break;
            }
            case JMP:
            {
                spu->ip++;
                DoJMP(spu);
                break;
            }
            case JE:
            {
                spu->ip++;
                DoJE(spu);
                break;
            }
            case JA:
            {
                spu->ip++;
                DoJA(spu);
                break;
            }
            case JB:
            {
                spu->ip++;
                DoJB(spu);
                break;
            }
            case JBE:
            {
                spu->ip++;
                DoJBE(spu);
                break;
            }
            case JAE:
            {
                spu->ip++;
                DoJAE(spu);
                break;
            }
            case JNE:
            {
                spu->ip++;
                DoJNE(spu);
                break;
            }
            case CALL:
            {
                spu->ip++;
                DoCALL(spu);
                break;
            }
            case RET:
            {
                DoRET(spu);
                break;
            }
            case WRAM:
            {
                DoWRAM(spu);
                break;
            }
            case FREE:
            {
                DoFREE(spu);
                break;
            }
            default:
            {
                printf("AAAA\n");
                break;
            }
        }
        //ProcDump(spu);
        //getchar();
        spu->ip++;
    }
}
void DoFREE(SPU * spu)
{
    MY_ASSERT(spu);

    for (int i = 0; i < RAM_SIZE; i++)
    {
        spu->ram[i] = 0;
    }
}

void DoWRAM(SPU * spu)
{
    MY_ASSERT(spu);

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
            return;
        }
    }
}

void DoCALL(SPU * spu)
{
    MY_ASSERT(spu);

    StackPush(spu->recursion_stk, spu->ip + 1);

    spu->ip = (int)(spu->code[spu->ip]) - 1;
}

void DoRET(SPU * spu)
{
    MY_ASSERT(spu);

    spu->ip = StackPop(spu->recursion_stk) - 1;
}

void DoPUSH (SPU * spu)
{
    MY_ASSERT(spu);

    //

    switch ((int)(spu->code[spu->ip - 1]) & 0xE0)
    {
        case NUM:
        {
            StackPush(spu->stk, spu->code[spu->ip]);

            break;
        }
        case REG:
        {
            StackPush(spu->stk, spu->registers[(int)(spu->code[spu->ip]) - 1]);

            break;
        }
        case RAM + NUM:
        {
            StackPush(spu->stk, spu->ram[(int)(spu->code[spu->ip])]);

            break;
        }
        case RAM + REG:
        {
            StackPush(spu->stk, spu->ram[(int)(spu->registers[(int)(spu->code[spu->ip]) - 1])]);

            break;
        }
        default:
        {
            printf("ERROR PUSH ARG\n"
                   "ip = %d\n"
                   "code[%d] = %lg", spu->ip, spu->ip, spu->code[spu->ip]);
            exit(ERROR_PUSH_ARG);
        }
    }
}

void DoPOP (SPU * spu)
{
    MY_ASSERT(spu);

    switch ((int)(spu->code[spu->ip - 1]) & 0xE0)
    {
        case NUM:
        {
            StackPop(spu->stk);

            break;
        }
        case REG:
        {
            StackType var = StackPop(spu->stk);

            spu->registers[(int)(spu->code[spu->ip]) - 1] = var;

            break;
        }
        case RAM + NUM:
        {
            StackType var = StackPop(spu->stk);
            spu->ram[(int)(spu->code[spu->ip])] = var;

            break;
        }
        case RAM + REG:
        {
            StackType var = StackPop(spu->stk);
            spu->ram[(int)(spu->registers[(int)(spu->code[spu->ip]) - 1])] = var;

            break;
        }
        default:
        {
            printf("ERROR POP ARG\n"
                   "ip = %d\n"
                   "code[%d] = %lg", spu->ip, spu->ip, spu->code[spu->ip]);
            exit(ERROR_PUSH_ARG);
        }
    }
}

void DoADD(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    StackPush(spu->stk, a + b);
}

void DoSQRT(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);

    StackPush(spu->stk, sqrt(a));
}

void DoSUB(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    StackPush(spu->stk, b - a);
}

void DoMUL(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    StackPush(spu->stk, a * b);
}

void DoDIV(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    StackPush(spu->stk, b / a);
}

void DoOUT(SPU * spu)
{
    MY_ASSERT(spu);

    printf("%lg\n", StackPop(spu->stk));
}

void DoHLT(SPU * spu)
{
    MY_ASSERT(spu);

    spu->hlt_status = 1;
}

void DoJMP(SPU * spu)
{
    MY_ASSERT(spu);

    spu->ip = (int)(spu->code[spu->ip]) - 1;
}

void DoJA(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    if (b > a)
    {
        spu->ip = (int)(spu->code[spu->ip]) - 1;
    }
}

void DoJB(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    if (b < a)
    {
        spu->ip = (int)(spu->code[spu->ip]) - 1;
    }
}

void DoJE(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    if (b == a)
    {
        spu->ip = (int)(spu->code[spu->ip]) - 1;
    }
}

void DoJAE(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    if (b >= a)
    {
        spu->ip = (int)(spu->code[spu->ip]) - 1;
    }
}

void DoJBE(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    if (b <= a)
    {
        spu->ip = (int)(spu->code[spu->ip]) - 1;
    }
}

void DoJNE(SPU * spu)
{
    MY_ASSERT(spu);

    StackType a = StackPop(spu->stk);
    StackType b = StackPop(spu->stk);

    if (b != a)
    {
        spu->ip = (int)(spu->code[spu->ip]) - 1;
    }
}

void DoIN(SPU * spu)
{
    MY_ASSERT(spu);

    printf("Enter:\n");

    StackType var = 0;

    scanf("%lg", &var);

    StackPush(spu->stk, var);
}

void ProcDump(SPU * spu)
{
    printf("%s\n=======================================================================\n%s", YELLOW, RESET_COLOR);

    printf("%sip%s%s:%s   ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    for (unsigned int i = 0; i < 11 && spu->ip + i < spu->size; i++)
    {
        if(i + spu->ip == spu->ip)
        {
            printf("%s%5d %s", RED, i + spu->ip, RESET_COLOR);
            continue;
        }
        printf(CYAN "%5d " RESET_COLOR, i + spu->ip);
    }

    printf("\n%scode%s%s:%s ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    for (unsigned int i = 0; i < 11 && spu->ip + i < spu->size; i++)
    {
        if(i + spu->ip == spu->ip)
        {
            printf("%s%5lg %s",  RED, spu->code[i + spu->ip], RESET_COLOR);
            continue;
        }
        printf("%s%5lg %s",  CYAN, spu->code[i + spu->ip], RESET_COLOR);
    }
    printf("\n");

    printf("%s~~~~~~~~~~^%s", MAGN, RESET_COLOR);

    printf("\n%sstk%s%s:%s  ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    DataDump(spu->stk);

    printf("\n%srec_stk%s%s:%s  ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    DataDump(spu->recursion_stk);

    printf("%s\nHLT Status: %d%s\n", BLUE, spu->hlt_status, RESET_COLOR);

    printf("%sregs%s:%s%s ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    for (int i = 0; i < 4; i++)
    {
        printf("%s%5lg %s",  CYAN, spu->registers[i], RESET_COLOR);
    }

    printf("\n%sram%s:%s%s  ", BLUE, RESET_COLOR, YELLOW, RESET_COLOR);

    for (int i = 0; i < RAM_SIZE; i++)
    {
        printf("%s%5lg %s",  CYAN, spu->ram[i], RESET_COLOR);
    }

    printf("%s\n=======================================================================\n%s", YELLOW, RESET_COLOR);
}



