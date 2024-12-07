#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys\stat.h>
#include "Asmh.h"

// assembler
    // str .cpp
        // main.cpp ...
    // inc .h
        // asm.h    ...

void AsmCtor(ASM * assembler, char * In_name, char * Out_name)
{
    MY_ASSERT(assembler);

    assembler->Infile      = NULL;
    assembler->Infile_name = In_name;

    assembler->buff = NULL;

    assembler->symb_count = 0;
    assembler->str_count  = 0;

    assembler->lables  = (LABLE*)calloc(MAX_LABEL_COUNT, sizeof(LABLE));
    MY_ASSERT(assembler->lables);

    assembler->lable_count = 0;

    assembler->machine_code = NULL;
    assembler->ip = 0;

    assembler->Outfile      = NULL;
    assembler->Outfile_name = Out_name;
}

void AsmDtor(ASM * assembler)
{
    MY_ASSERT(assembler);

    assembler->Infile      = NULL;
    assembler->Infile_name = NULL;

    free(assembler->buff);
    assembler->buff = NULL;

    free(assembler->num_buff);
    assembler->num_buff = NULL;

    assembler->symb_count = 0;
    assembler->str_count  = 0;

    free(assembler->lables);
    assembler->lables = NULL;

    assembler->lable_count = 0;

    free(assembler->machine_code);
    assembler->machine_code = NULL;

    assembler->ip = 0;

    assembler->Outfile      = NULL;
    assembler->Outfile_name = NULL;
}

void ParseStr(ASM * assembler)
{
    MY_ASSERT(assembler);

    _FOPEN(assembler->Infile, assembler->Infile_name, "rb");

    assembler->symb_count = GetFileSymbolsCount(assembler->Infile);

    assembler->buff = (char*)calloc(assembler->symb_count + 1, sizeof(char));
    MY_ASSERT(assembler->buff);

    if (fread((void*)assembler->buff, sizeof(char), assembler->symb_count, assembler->Infile) != assembler->symb_count)
    {
        printf("ERROR READ FROM FILE\n");

        exit (0);
    }

    _FCLOSE(assembler->Infile);

    assembler->str_count = GetStrCount(assembler->buff);

    printf("=====%d=====\n", assembler->str_count);

    assembler->machine_code = (CMD*)calloc(assembler->str_count, sizeof(CMD));
    MY_ASSERT(assembler->machine_code);

    assembler->num_buff = (int*)calloc(assembler->str_count * 2, sizeof(int));
    MY_ASSERT(assembler->num_buff);

    GetCodeStr(assembler);
}

size_t GetFileSymbolsCount(FILE * file)
{
    struct stat st;

    if (fstat(fileno(file), &st) == 0)
    {
        return st.st_size;
    }

    return 0;
}

int GetStrCount(char * buff)
{
    MY_ASSERT(buff);

    int str_count = 0;

    while (*buff != '\0')
    {
        if (*buff == '\r')
        {
            *buff       = '\0';
            *(buff + 1) = '\0';

            buff++;

            str_count++;
        }

        buff++;
    }

    return str_count;
}

int CheckLable(char * str)
{
    int c = 0;

    while (*str != '\0')
    {
        if (*str == ':' && c != 0)
        {
            return 1;
        }

        c++;
        str++;
    }

    return 0;
}

void GetCodeStr(ASM * assembler)
{
    MY_ASSERT(assembler);

    int ip = 0;

    for (size_t i = 0, j = 0; i < assembler->str_count; i++)
    {
        if (assembler->buff[j] == '\0')
        {
            assembler->machine_code[i].str_num = i + 1;
            assembler->machine_code[i].str = assembler->buff + j;
            assembler->machine_code[i].len_str = 0;

            j += 2;

            continue;
        }

        if (CheckLable(assembler->buff + j))
        {
            assembler->machine_code[i].str_num = i + 1;
            assembler->machine_code[i].str = assembler->buff + j;
            assembler->machine_code[i].len_str = strlen(assembler->buff + j) - 1;
            assembler->machine_code[i].cmd = LABEL;

            j += strlen((const char*)(assembler->buff + j)) + 2;

            *(assembler->buff + j - 3) = '\0';

            SpaceSkip(&assembler->machine_code[i]);

            assembler->lables[assembler->lable_count].str = assembler->machine_code[i].str;
            assembler->lables[assembler->lable_count].ip = ip;

            assembler->lable_count++;

            continue;
        }
        if (strcmp((const char *)&assembler->buff[j], "POP") == 0) ip++;

        for (size_t k = j; k < assembler->symb_count; k++)
        {
            if (assembler->buff[k] == ' ') ip++;

            if (assembler->buff[k] == '\0')
            {
                assembler->machine_code[i].str_num = i + 1;
                assembler->machine_code[i].str = assembler->buff + j;
                assembler->machine_code[i].len_str = k - j;

                k += 2;

                j = k;

                ip++;

                SpaceSkip(&assembler->machine_code[i]);

                break;
            }
        }
    }

    assembler->ip = ip;
}

/*
value
reg
ram

[100] - intov

push [49]

[ ]

get_value(, 0\1)

strtod

get_reg(, 0\1)

R_X
*/

void GetArg(ASM * assembler, int i)
{
    MY_ASSERT(assembler);

    switch(assembler->machine_code[i].cmd)
    {
        case PUSH:
        {
            if (isdigit(*(assembler->machine_code[i].str + 5)))
            {
                assembler->machine_code[i].arg = atoi((const char*)(assembler->machine_code[i].str + 5));
                assembler->machine_code[i].cmd = PUSHN;
            }
            else if (*(assembler->machine_code[i].str + 5) == 'R' && *(assembler->machine_code[i].str + 7) == 'X'
                     && *(assembler->machine_code[i].str + 8) == '\0' && *(assembler->machine_code[i].str + 6) - 'A' + 1 <= 4)
            {
                assembler->machine_code[i].reg = (REGISTERS)(*(assembler->machine_code[i].str + 6) - 'A' + 1);
                assembler->machine_code[i].cmd = PUSHR;
            }
            else
            {
                printf("ERROR PUSH ARG\n"
                       "LINE: %d\n"
                       "STR: %s\n", assembler->machine_code[i].str_num, assembler->machine_code[i].str);
                exit (0);
            }

            break;
        }

        case POP:
        {
            if (assembler->machine_code[i].len_str == 3)
            {
                assembler->machine_code[i].reg = REG_DEFAULT;
            }
            else if (*(assembler->machine_code[i].str + 4) == 'R' && *(assembler->machine_code[i].str + 6) == 'X'
                     && *(assembler->machine_code[i].str + 7) == '\0' && *(assembler->machine_code[i].str + 5) - 'A' + 1 <= 4)
            {
                assembler->machine_code[i].reg = (REGISTERS)(*(assembler->machine_code[i].str + 5) - 'A' + 1);
            }
            else
            {
                printf("ERROR POP ARG\n"
                       "LINE: %d\n"
                       "STR: %s\n", assembler->machine_code[i].str_num, assembler->machine_code[i].str);
                exit (0);
            }

            break;
        }
        case JMP:
        {
            if (isdigit(*(assembler->machine_code[i].str + 4)))
            {
                assembler->machine_code[i].arg = atoi((const char*)(assembler->machine_code[i].str + 4));
            }
            else if (isalpha(*(assembler->machine_code[i].str + 4)))
            {
                for (int j = 0; j < assembler->lable_count; j++)
                {
                    if (strcmp((const char*)(assembler->machine_code[i].str + 4), (const char*)assembler->lables[j].str) == 0)
                    {
                        assembler->machine_code[i].arg = assembler->lables[j].ip;

                        break;
                    }
                }
            }
            else
            {
                printf("ERROR JMP ARG\n"
                       "LINE: %d\n"
                       "STR: %s\n", assembler->machine_code[i].str_num, assembler->machine_code[i].str);
                exit (0);
            }

            break;
        }
        case JB:
        {
            if (isdigit(*(assembler->machine_code[i].str + 4)))
            {
                assembler->machine_code[i].arg = atoi((const char*)(assembler->machine_code[i].str + 4));
            }
            else if (isalpha(*(assembler->machine_code[i].str + 4)))
            {
                for (int j = 0; j < assembler->lable_count; j++)
                {
                    if (strcmp((const char*)(assembler->machine_code[i].str + 4), (const char*)assembler->lables[j].str) == 0)
                    {
                        assembler->machine_code[i].arg = assembler->lables[j].ip;

                        break;
                    }
                }
            }
            else
            {
                printf("ERROR JMP ARG\n"
                       "LINE: %d\n"
                       "STR: %s\n", assembler->machine_code[i].str_num, assembler->machine_code[i].str);
                exit (0);
            }

            break;
        }
        case JA:
        {
            if (isdigit(*(assembler->machine_code[i].str + 4)))
            {
                assembler->machine_code[i].arg = atoi((const char*)(assembler->machine_code[i].str + 4));
            }
            else if (isalpha(*(assembler->machine_code[i].str + 4)))
            {
                for (int j = 0; j < assembler->lable_count; j++)
                {
                    if (strcmp((const char*)(assembler->machine_code[i].str + 4), (const char*)assembler->lables[j].str) == 0)
                    {
                        assembler->machine_code[i].arg = assembler->lables[j].ip;

                        break;
                    }
                }
            }
            else
            {
                printf("ERROR JMP ARG\n"
                       "LINE: %d\n"
                       "STR: %s\n", assembler->machine_code[i].str_num, assembler->machine_code[i].str);
                exit (0);
            }

            break;
        }
        case JE:
        {
            if (isdigit(*(assembler->machine_code[i].str + 4)))
            {
                assembler->machine_code[i].arg = atoi((const char*)(assembler->machine_code[i].str + 4));
            }
            else if (isalpha(*(assembler->machine_code[i].str + 4)))
            {
                for (int j = 0; j < assembler->lable_count; j++)
                {
                    if (strcmp((const char*)(assembler->machine_code[i].str + 4), (const char*)assembler->lables[j].str) == 0)
                    {
                        assembler->machine_code[i].arg = assembler->lables[j].ip;

                        break;
                    }
                }
            }
            else
            {
                printf("ERROR JMP ARG\n"
                       "LINE: %d\n"
                       "STR: %s\n", assembler->machine_code[i].str_num, assembler->machine_code[i].str);
                exit (0);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}

void GetCodeInfo(ASM * assembler)
{
    MY_ASSERT(assembler);

    int ip = 0;

    for (size_t i = 0; i < assembler->str_count; i++)
    {
        if (assembler->machine_code[i].len_str == 0)
        {
            assembler->machine_code[i].personal_ip = ip;
        }
        else if (assembler->machine_code[i].cmd == LABEL)
        {
            continue;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "PUSH ", 5) == 0)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = PUSH;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            if (assembler->machine_code[i].reg == 0)
                assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;
            else
                assembler->num_buff[ip + 1] = assembler->machine_code[i].reg;

            ip += 2;

        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "ADD") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = ADD;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "SUB") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = SUB;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "MUL") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = MUL;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "DIV") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = DIV;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "OUT") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = OUT;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "HLT") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = HLT;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JMP ", 4) == 0)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = JMP;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JA ", 3) == 0)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = JA;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JB ", 3) == 0)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = JB;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JE ", 3) == 0)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = JE;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "IN") == 0 && assembler->machine_code[i].len_str == 2)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = IN;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "POP", 3) == 0 && (*(assembler->machine_code[i].str + 3) == '\0' || *(assembler->machine_code[i].str + 3) == ' '))
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = POP;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].reg;

            ip += 2;

        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "SQRT") == 0 && assembler->machine_code[i].len_str == 4)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = SQRT;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "CALL ", 5) == 0)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = CALL;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "RET") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].personal_ip = ip;
            assembler->machine_code[i].cmd = RET;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else
        {
            printf("UNKNOWN COMMAND\n"
                   "LINE: %d\n"
                   "STR: %s\n", assembler->machine_code[i].str_num, assembler->machine_code[i].str);

            exit (COMMAND_ERROR);
        }
    }

    assembler->ip = ip;
}

void SpaceSkip(CMD * code)
{
    MY_ASSERT(code);

    while (!isalpha(*(code->str)))
    {
        code->str++;
        code->len_str--;
    }
}

void WriteInFile(ASM * assembler)
{
    _FOPEN(assembler->Outfile, assembler->Outfile_name, "wb");

    printf("%d %d\n", fwrite(assembler->num_buff, sizeof(int), assembler->ip, assembler->Outfile), assembler->ip);

    _FCLOSE(assembler->Outfile);
}
