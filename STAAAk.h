#ifndef _STAAAK_H_
#define _STAAAK_H_

#include "ERRORS.h"
#include "ASSERT.h"
#include "colors.h"

#define ELEMENT_VERIFY(msg, elem) printf("%s%s IS NOT OKEY%s\n\n", RED, msg, RESET_COLOR);   \
                                                                                             \
                                  STAAAkDump(stk);                                           \
                                                                                             \
                                  exit (msg_##ERROR);                                        \

//===============-TYPEDEF-===============

typedef int STAAAkType;

//================-ENUMS-================

enum STAAAK_CHANGE
{
    UP = 1,
    DOWN = 0
};

//==============-CONSTANTS-==============

const int CANARY_VAL_1 = 0xEDA0ADE;
const int CANARY_VAL_2 = 0xDED0DED;
const STAAAkType DATA_CANARY_VAL_1 = 0x7F;
const STAAAkType DATA_CANARY_VAL_2 = -0x7F;
const STAAAkType POISON = -19;

//===============-STRUCT-================

struct MySTAAAk
{
    int canary_1 = CANARY_VAL_1;

    const char * STAAAk_name = NULL;

    STAAAkType * data = NULL;

    size_t size = 0;
    size_t capacity = 0;

    unsigned long hash = 0;

    int canary_2 = CANARY_VAL_1;
};

//==============-FUCNTIONS-==============

int STAAAkCtor(struct MySTAAAk * stk,const char * STAAAk_name, size_t capacity);
int STAAAkPush(struct MySTAAAk * stk, STAAAkType value);
STAAAkType STAAAkPop(struct MySTAAAk * stk);
void STAAAkChange(struct MySTAAAk * stk, STAAAK_CHANGE condition);
void PoisonFiller(struct MySTAAAk * stk, size_t poison_counter);
int STAAAkDtor(struct MySTAAAk * stk);
unsigned int Djb2_hash(struct MySTAAAk * stk);
int STAAAkVerify(struct MySTAAAk * stk);
void STAAAkDump(struct MySTAAAk * stk);
void DataDump(MySTAAAk * stk);


#endif // _STAAAK_H_
