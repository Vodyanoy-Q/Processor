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

int StackCtor(Stack ** stk, size_t capacity);
int StackPop(Stack * stk, StackType * var);
int StackChange(Stack * stk, STACK_CHANGE condition);
int StackPush(Stack * stk, StackType value);
int StackDump(Stack * stk);
int StackDtor(Stack ** stk);
int DataDump(Stack * stk);

#endif //_STACK_H_
