#include <stdio.h>
#include <stdlib.h>
#include "Asmh.h"

int main()
{
    _FOPEN(file, "text.txt", "rb");

    size_t size = GetFileSize(file);

    char * buff = (char*)calloc(size + 1, sizeof(char));

    fread((void*)buff, sizeof(char), size, file);

    fclose(file);

    size_t str_count = GetStrSize(buff, size);

    printf("%d\n", str_count);

    CMD * machine_code = GetCodeInfo(buff, str_count, size);

    for(size_t i = 0; i < str_count; i++)
    {
        printf("str = %s\n"
               "len_str = %d\n"
               "cmd = %d\n"
               "arg = %d\n"
               "reg = %d\n\n", machine_code[i].str, machine_code[i].len_str, machine_code[i].cmd, machine_code[i].arg, machine_code[i].reg);
    }

    WriteInFile(machine_code, str_count);

    free(buff);
    free(machine_code);

    return 0;
}
