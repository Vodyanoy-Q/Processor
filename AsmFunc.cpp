#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Asmh.h"

size_t GetFileSize(FILE * file)
{
    MY_ASSERT(file);

    fseek(file, 0L, SEEK_END);

    size_t symbols = ftell(file);

    rewind(file);

    return symbols;
}

size_t GetStrSize(char * buff, size_t size)
{
    size_t str_count = 0;

    for (size_t i = 0; i < size; i++)
    {
        if(buff[i] == '\r')
        {
            buff[i]     = '\0';
            buff[i + 1] = '\0';
            i++;

            str_count++;
        }
    }

    return str_count;
}

CMD * GetCodeInfo(char * buff, size_t str_count, size_t size)
{
    CMD * machine_code = (CMD*)calloc(str_count, sizeof(CMD));

    for (size_t i = 0, k = 1; i < str_count; i++)
    {
        for (size_t j = k; j < size; j++)
        {
            if (buff[j] == '\0')
            {
                machine_code[i].str = buff + k - 1;

                j++;

                machine_code[i].len_str = j - k;

                j += 2;

                k = j;

                SpaceSkip(&machine_code[i]);

                break;
            }
        }
    }

    for (size_t i = 0; i < str_count; i++)
    {
        if(machine_code[i].len_str == 1)
        {
            machine_code[i + 1].str -= 1;
        }
        else if (strncmp((const char*)machine_code[i].str, "PUSH ", 5) == 0)
        {
            machine_code[i].cmd = PUSH;

            if (isdigit(*(machine_code[i].str + 5)))
            {
                machine_code[i].arg = atoi((const char*)(machine_code[i].str + 5));
            }
            else
            {
                if (*(machine_code[i].str + 5) == 'R' && *(machine_code[i].str + 6) < 'D')
                {
                    machine_code[i].reg = (REGISTERS)(*(machine_code[i].str + 6) - 'A' + 1);
                }
                else
                {
                    printf("ERROR PUSH ARG\n");
                    exit(0);
                }
            }
        }
        else if (strcmp((const char*)machine_code[i].str, "ADD") == 0)
        {
            machine_code[i].cmd = ADD;
        }
        else if (strcmp((const char*)machine_code[i].str, "SUB") == 0)
        {
            machine_code[i].cmd = SUB;
        }
        else if (strcmp((const char*)machine_code[i].str, "MUL") == 0)
        {
            machine_code[i].cmd = MUL;
        }
        else if (strcmp((const char*)machine_code[i].str, "DIV") == 0)
        {
            machine_code[i].cmd = DIV;
        }
        else if (strcmp((const char*)machine_code[i].str, "OUT") == 0)
        {
            machine_code[i].cmd = OUT;
        }
        else if (strcmp((const char*)machine_code[i].str, "HLT") == 0)
        {
            machine_code[i].cmd = HLT;
        }
    }

    return machine_code;
}

void SpaceSkip(CMD * code)
{
    while(*(code->str) == ' ')
    {
        code->str = code->str + 1;
        code->len_str = code->len_str - 1;
    }
}

void WriteInFile(CMD * machine_code, size_t str_count)
{
    _FOPEN(file, "code.asm", "w+")

    for (size_t i = 0; i < str_count; i++)
    {
        switch (machine_code[i].cmd)
        {
            case PUSH:
            {
                fprintf(file, "%d ", PUSH);

                if (machine_code[i].reg == 0)
                {
                    fprintf(file, "%d ", NUM);
                    fprintf(file, "%d ", machine_code[i].arg);
                }
                else
                {
                    fprintf(file, "%d ", REG);
                    fprintf(file, "%d ", machine_code[i].reg);
                }

                break;
            }
            case ADD:
            {
                fprintf(file, "%d ", ADD);
                break;
            }
            case SUB:
            {
                fprintf(file, "%d ", SUB);
                break;
            }
            case MUL:
            {
                fprintf(file, "%d ", MUL);
                break;
            }
            case DIV:
            {
                fprintf(file, "%d ", DIV);
                break;
            }
            case OUT:
            {
                fprintf(file, "%d ", OUT);
                break;
            }
            case HLT:
            {
                fprintf(file, "%d ", HLT);
                break;
            }
            case CMD_DEFAULT:
            {
                break;
            }

            default:
            {
                printf("ERROR COMMAND\n");

                exit (COMMAND_ERROR);
            }
        }
    }

    fclose(file);
}




