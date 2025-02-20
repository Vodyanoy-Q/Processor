#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#define MAX_LABEL_COUNT 20

#include "ASSERT.h"
#include "commands.h"

enum ERRORS
{
    ADDRES_ERROR     = -1,
    OPEN_FILE_ERROR  = -2,
    FILE_CLOSE_ERROR = -3,
    COMMAND_ERROR    = -4,
    RAM_ARG_ERROR    = -5,
    GET_ARG_ERROR    = -6,
    ARG_ERROR        = -7
};

enum REGISTERS
{
    REG_DEFAULT = 0,

    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4
};

enum RAM
{
    NOT_RAM = 0,
    IS_RAM  = 1
};

struct CMD
{
    int str_num = 0;

    char * str = NULL;
    size_t len_str = 0;
    size_t cmd_len = 0;

    int personal_ip = 0;
    unsigned char cmd = CMD_DEFAULT;
    double arg = 0;
    char reg = REG_DEFAULT;
};

struct LABLE
{
    char * str = NULL;
    int ip = 0;
};

struct ASM
{

    FILE * Infile      = NULL;
    char * Infile_name = NULL;

    char * buff = NULL;

    size_t symb_count = 0;
    size_t str_count  = 0;

    LABLE * lables  = NULL;
    int lable_count = 0;

    CMD * machine_code = NULL;
    size_t ip = 0;
    double * num_buff = NULL;

    FILE * Outfile      = NULL;
    char * Outfile_name = NULL;
};


size_t GetFileSymbolsCount(FILE * file);
void GetArg(ASM * assembler, int i);
int CheckLable(char * str);
int GetStrCount(char * buff);
void GetCodeInfo(ASM * assembler);
void GetArg(CMD * machine_code);
void GetCodeStr(ASM * assembler);
void AsmCtor(ASM * assembler, char * In_name, char * Out_name);
void AsmDtor(ASM * assembler);
void SpaceSkip(CMD * code, size_t * j);
void ParseStr(ASM * assembler);
int GetValue(CMD * machine_code, int ram_status);
int GetReg(CMD * machine_code, int ram_status);
int GetLabel(ASM * assembler, int i);
void WriteInFile(ASM * assembler);

#endif //_ASSEMBLER_H_
