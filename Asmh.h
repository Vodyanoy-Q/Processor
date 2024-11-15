#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#define MY_ASSERT(addres) if(!addres)                                                        \
                          {                                                                  \
                              printf(" ERROR\n line %d\n file %s\n func %s\n",               \
                                     __LINE__, __FILE__, __func__);                          \
                                                                                             \
                              exit (ADDRES_ERROR);                                           \
                          }                                                                  \

#define _FOPEN(addres, name, arg) FILE *addres = fopen(name, arg);                           \
                                                                                             \
                                 if(addres == 0)                                             \
                                 {                                                           \
                                     printf("ERROR OPEN FILE\n"                              \
                                            "PLEASE CHECK FILE AND TRY AGAIN\n");            \
                                                                                             \
                                     exit (OPEN_FILE_ERROR);                                 \
                                 }

enum ERRORS
{
    ADDRES_ERROR = -1,
    OPEN_FILE_ERROR = -2,
    FILE_CLOSE_ERROR = -3,
    COMMAND_ERROR = -4,
};

enum REGISTERS
{
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4,

    REG_DEFAULT = -1,
};

enum ARG
{
    REG = 0,
    NUM = 1,
};

enum CMD_CODE
{
    PUSH = 1,
    ADD  = 2,
    SUB  = 3,
    MUL  = 4,
    DIV  = 5,
    OUT  = 6,
    HLT  = 7,

    CMD_DEFAULT = 0,
};

struct CMD
{
    char * str = NULL;
    size_t len_str = 0;
    CMD_CODE cmd = CMD_DEFAULT;
    int arg = 0;
    REGISTERS reg = REG_DEFAULT;
};

size_t GetFileSize(FILE * file);
size_t GetStrSize(char * buff, size_t size);
void WriteInFile(CMD * machine_code, size_t str_count);
CMD * GetCodeInfo(char * buff, size_t str_count, size_t size);
void SpaceSkip(CMD * code);

#endif //_ASSEMBLER_H_
