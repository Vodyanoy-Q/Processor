#include <stdio.h>
#include <stdlib.h>
#include "Asmh.h"

int main(int argc, char * argv[])
{
    ASM assembler = {};

    AsmCtor(&assembler, argv[1], argv[2]);

    ParseStr(&assembler);

    GetCodeInfo(&assembler);

    for(size_t i = 0; i < assembler.str_count; i++)
    {
        printf("str_num = %d\n"
               "str = %s\n"
               "len_str = %d\n"
               "personal_ip = %d\n"
               "cmd = %d\n"
               "arg = %d\n"
               "reg = %d\n\n", assembler.machine_code[i].str_num, assembler.machine_code[i].str, assembler.machine_code[i].len_str, assembler.machine_code[i].personal_ip, assembler.machine_code[i].cmd, assembler.machine_code[i].arg, assembler.machine_code[i].reg);
    }

    //MakeOutBuff(&assembler);


    printf("\n=========================================================================\n");

    for (size_t i = 0; i < assembler.ip; i++)
    {
        printf("%5d", assembler.num_buff[i]);
    }

    printf("\n=========================================================================\n");

    printf("%d\n", assembler.ip);

    WriteInFile(&assembler);

    printf("=========================================================================\n");

    for(size_t i = 0; i < assembler.str_count; i++)
    {
        printf("i = %d, ptr = %d\n", i + 1, assembler.machine_code[i].str);

        for(size_t j = 0; j <= assembler.machine_code[i].len_str; j++)
        {
            printf("%3c", *(assembler.machine_code[i].str + j));
        }

        printf("\n");

        for(size_t j = 0; j <= assembler.machine_code[i].len_str; j++)
        {
            printf("%3d", *(assembler.machine_code[i].str + j));
        }
        printf("\n");
    }

    printf("=========================================================================\n");

    for (int i = 0; i < assembler.lable_count; i++)
    {
        printf("i = %d ip = %d\n", i + 1, assembler.lables[i].ip);

        for(size_t j = 0; *(assembler.lables[i].str + j) != '\0'; j++)
        {
            printf("%3c ", *(assembler.lables[i].str + j));
        }

        printf("\n");

        for(size_t j = 0; *(assembler.lables[i].str + j) != '\0'; j++)
        {
            printf("%3d", *(assembler.lables[i].str + j));
        }
        printf("\n");

    }

    AsmDtor(&assembler);

    return 0;
}
