#ifndef _STACK_H_
#define _STACK_H_

#include "ASSERT.h"
#include "ERRORS.h"

typedef double StackType;

enum STACK_CHANGE
{
    UP = 1,
    DOWN = 0
};

struct Stack
{
    StackType * data = NULL;

    size_t size = 0;
    size_t capacity = 0;
};

Stack * StackCtor(size_t capacity);
StackType StackPop(Stack * stk);
void StackChange(Stack * stk, STACK_CHANGE condition);
int StackPush(Stack * stk, StackType value);
void StackDump(Stack * stk);
void StackDtor(Stack ** stk);
void DataDump(Stack * stk);

#endif //_STACK_H_
