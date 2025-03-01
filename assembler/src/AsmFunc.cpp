#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "../inc/Asmh.h"

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
        AsmDtor(assembler);

        exit (0);
    }

    _FCLOSE(assembler->Infile);

    assembler->str_count = GetStrCount(assembler->buff);

    assembler->machine_code = (CMD*)calloc(assembler->str_count, sizeof(CMD));
    MY_ASSERT(assembler->machine_code);

    assembler->num_buff = (double*)calloc(assembler->str_count * 2, sizeof(double));
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
        else if (*buff == '\n')
        {
            *buff = '\0';

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
        assembler->machine_code[i].personal_ip = ip;
        assembler->machine_code[i].str_num = i + 1;
        assembler->machine_code[i].str = assembler->buff + j;

        if (assembler->buff[j] == '\0')
        {
            j += 1;

            continue;
        }

        SpaceSkip(&assembler->machine_code[i], &j);


        if (CheckLable(assembler->buff + j))
        {
            assembler->machine_code[i].len_str = strlen(assembler->buff + j) - 1;
            assembler->machine_code[i].cmd = LABEL;

            j += strlen((const char*)(assembler->buff + j)) + 1;

            *(assembler->buff + j - 2) = '\0';

            assembler->lables[assembler->lable_count].str = assembler->machine_code[i].str;
            assembler->lables[assembler->lable_count].ip = ip;

            assembler->lable_count++;

            continue;
        }
        if (strcmp((const char *)&assembler->buff[j], "POP") == 0)
        {
            ip++;
            assembler->machine_code[i].cmd_len = 3;
        }

        for (size_t k = j; k < assembler->symb_count; k++)
        {
            if (assembler->buff[k] == ' ')
            {
                assembler->machine_code[i].cmd_len = k - j;
                ip++;
            }

            if (assembler->buff[k] == '\0')
            {
                assembler->machine_code[i].len_str = k - j;

                k += 1;

                j = k;

                ip++;

                break;
            }
        }
    }

    assembler->ip = ip;
}

void GetArg(ASM * assembler, int i)
{
    MY_ASSERT(assembler);

    if (assembler->machine_code[i].cmd_len == assembler->machine_code[i].len_str)
    {
        assembler->machine_code[i].arg  = 0;
        assembler->machine_code[i].cmd |= NUM;

        return;
    }

    int ram_status = 0;

    if (*(assembler->machine_code[i].str + assembler->machine_code[i].cmd_len + 1) == '[')
    {
        if (assembler->machine_code[i].str[assembler->machine_code[i].len_str - 1] == ']' &&
                assembler->machine_code[i].str[assembler->machine_code[i].len_str] == '\0')
        {
            ram_status = 1;
            if (GetValue(&assembler->machine_code[i], ram_status)){}
            else if (GetReg(&assembler->machine_code[i], ram_status)){}
            else
            {
                printf("ERROR RAM ARG\n"
                       "LINE: %d\n"
                       "STR: %s\n", i + 1, assembler->machine_code[i].str);
            }

            assembler->machine_code[i].cmd |= RAM;
        }
        else
        {
            printf("ERROR RAM ARG\n"
                   "LINE: %d\n"
                   "STR: %s\n", i + 1, assembler->machine_code[i].str);
        }
    }
    else if (GetValue(&assembler->machine_code[i], ram_status))
    {
        if (*assembler->machine_code[i].str == 'J')
        {
            assembler->machine_code[i].arg = assembler->machine_code[(int)assembler->machine_code[i].arg].personal_ip;
            assembler->machine_code[i].cmd |= NUM;
        }
    }
    else if (GetReg(&assembler->machine_code[i], ram_status)) {}
    else if (GetLabel(assembler, i)){}
    else
    {
        printf("ERROR ARG\n"
               "LINE: %d\n"
               "STR: %s\n", i + 1, assembler->machine_code[i].str);
    }
}

int GetLabel(ASM * assembler, int i)
{
    MY_ASSERT(assembler);

    for (int j = 0; j < assembler->lable_count; j++)
    {
        if (strcmp(assembler->lables[j].str, assembler->machine_code[i].str + assembler->machine_code[i].cmd_len + 1) == 0)
        {
            assembler->machine_code[i].arg = assembler->lables[j].ip;
            assembler->machine_code[i].cmd |= NUM;

            return 1;
        }
    }

    return 0;
}

int GetValue(CMD * machine_code, int ram_status)
{
    MY_ASSERT(machine_code);

    char * check_ptr = NULL;

    machine_code->arg = strtod(machine_code->str + machine_code->cmd_len + 1 + ram_status, &check_ptr);

    if (*check_ptr != '\0' && *check_ptr != ']')
    {
        return 0;
    }

    machine_code->cmd |= NUM;

    return 1;
}

int GetReg(CMD * machine_code, int ram_status)
{
    MY_ASSERT(machine_code);

    char * check_ptr = machine_code->str + machine_code->cmd_len + 1 + ram_status;

    if (*(check_ptr) == 'R' && *(check_ptr + 2) == 'X' && *(check_ptr + 1) - 'A' < 4)
    {
        machine_code->arg  = *(check_ptr + 1) - 'A' + 1;
        machine_code->cmd |= REG;

        return *(check_ptr + 3 + ram_status) == '\0';
    }
    return 0;
}

void GetCodeInfo(ASM * assembler)
{
    MY_ASSERT(assembler);

    int ip = 0;

    for (size_t i = 0; i < assembler->str_count; i++)
    {
        if (assembler->machine_code[i].len_str == 0)
        {
            continue;
        }
        else if (assembler->machine_code[i].cmd == LABEL)
        {
            continue;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "PUSH ", 5) == 0)
        {
            assembler->machine_code[i].cmd = PUSH;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "ADD") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].cmd = ADD;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "SUB") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].cmd = SUB;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "MUL") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].cmd = MUL;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "DIV") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].cmd = DIV;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "OUT") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].cmd = OUT;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "HLT") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].cmd = HLT;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JMP ", 4) == 0)
        {
            assembler->machine_code[i].cmd = JMP;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JA ", 3) == 0)
        {
            assembler->machine_code[i].cmd = JA;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JB ", 3) == 0)
        {
            assembler->machine_code[i].cmd = JB;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JE ", 3) == 0)
        {
            assembler->machine_code[i].cmd = JE;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JBE ", 4) == 0)
        {
            assembler->machine_code[i].cmd = JBE;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JAE ", 4) == 0)
        {
            assembler->machine_code[i].cmd = JAE;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "JNE ", 4) == 0)
        {
            assembler->machine_code[i].cmd = JNE;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "IN") == 0 && assembler->machine_code[i].len_str == 2)
        {
            assembler->machine_code[i].cmd = IN;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "POP", 3) == 0 && (*(assembler->machine_code[i].str + 3) == '\0' || *(assembler->machine_code[i].str + 3) == ' '))
        {
            assembler->machine_code[i].cmd = POP;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;

        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "SQRT") == 0 && assembler->machine_code[i].len_str == 4)
        {
            assembler->machine_code[i].cmd = SQRT;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strncmp((const char*)assembler->machine_code[i].str, "CALL ", 5) == 0)
        {
            assembler->machine_code[i].cmd = CALL;

            GetArg(assembler, i);

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;
            assembler->num_buff[ip + 1] = assembler->machine_code[i].arg;

            ip += 2;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "RET") == 0 && assembler->machine_code[i].len_str == 3)
        {
            assembler->machine_code[i].cmd = RET;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "WRAM") == 0 && assembler->machine_code[i].len_str == 4)
        {
            assembler->machine_code[i].cmd = WRAM;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else if (strcmp((const char*)assembler->machine_code[i].str, "FREE") == 0 && assembler->machine_code[i].len_str == 4)
        {
            assembler->machine_code[i].cmd = FREE;

            assembler->num_buff[ip] = assembler->machine_code[i].cmd;

            ip++;
        }
        else
        {
            printf("UNKNOWN COMMAND\n"
                   "LINE: %d\n"
                   "STR: %s\n", assembler->machine_code[i].str_num, assembler->machine_code[i].str);

            //exit (COMMAND_ERROR);
        }
    }

    assembler->ip = ip;
}

void SpaceSkip(CMD * code, size_t * j)
{
    MY_ASSERT(code);

    while (!isalpha(*(code->str)))
    {
        code->str++;
        code->len_str--;
        *j = *j + 1;
    }
}

void WriteInFile(ASM * assembler)
{
    _FOPEN(assembler->Outfile, assembler->Outfile_name, "wb");

    fwrite(assembler->num_buff, sizeof(double), assembler->ip, assembler->Outfile);

    _FCLOSE(assembler->Outfile);
}
