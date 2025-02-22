#include <stdio.h>
#include <stdlib.h>

#include "../inc/Stack.h"
#include "../inc/colors.h"
#include "../inc/Proc.h"

int DataDump(Stack * stk)
{
    MY_ASSERT(stk, STK_ADDRESS_ERROR);

    for (unsigned int i = 0; i < stk->capacity; i++)
    {
        if (i < stk->size)
        {
            if (i == stk->size - 1)
            {
                printf(RED "%5lg " RESET_COLOR, stk->data[i]);
                continue;
            }
            printf(CYAN "%5lg " RESET_COLOR, stk->data[i]);
        }
        else
        {
            printf(GREEN "%5lg " RESET_COLOR, stk->data[i]);
        }
    }

    return NO_ERROR;
}

int StackCtor(Stack ** stk, size_t capacity)
{
    Stack * stack = (Stack*)calloc(1, sizeof(Stack));
    MY_ASSERT(stk, ADDRESS_ERROR);

    stack->size = 0;
    stack->capacity = capacity;

    stack->data = (StackType*)calloc(stack->capacity, sizeof(StackType));
    MY_ASSERT(stack->data, ADDRESS_ERROR);

    *stk = stack;

    return NO_ERROR;
}

int StackDtor(Stack ** stk)
{
    MY_ASSERT(*stk, STK_ADDRESS_ERROR);

    (*stk)->size = 0;
    (*stk)->capacity = 0;

    free((*stk)->data);
    (*stk)->data = NULL;

    free(*stk);

    *stk = NULL;

    return NO_ERROR;
}

int StackPush(Stack * stk, StackType value)
{
    MY_ASSERT(stk, STK_ADDRESS_ERROR);

    if (stk->size == stk->capacity)
    {
        int err = StackChange(stk, UP);

        if (err != 0)
        {
            return err;
        }
    }

    stk->data[stk->size] = value;

    stk->size += 1;

    return 0;
}

int StackDump(Stack * stk)
{
    MY_ASSERT(stk, STK_ADDRESS_ERROR);

    YELLOW_TEXT("==================================================\n");

    for (size_t i = 0; i < stk->capacity; i++)
    {
        printf(YELLOW "%5lg " RESET_COLOR, stk->data[i]);
    }

    YELLOW_TEXT("\n==================================================\n");

    return NO_ERROR;
}

int StackPop(Stack * stk, StackType * var)
{
    MY_ASSERT(stk, STK_ADDRESS_ERROR);
    MY_ASSERT(var, ADDRESS_ERROR);

    if (stk->size == 0)
    {
        RED_TEXT("ERROR: STACK IS EMPTY.\n");
        return POP_ERROR;
    }
    else
    {
        *var = stk->data[stk->size - 1];
        stk->data[stk->size - 1] = 0;

        stk->size -= 1;

        if (stk->size == 0)
        {
            stk->capacity = 1;

            return StackChange(stk, DOWN);
        }

        else if (stk->size > 0 && stk->capacity / 4 == stk->size)
        {
            return StackChange(stk, DOWN);
        }
    }

    return NO_ERROR;
}

int StackChange(Stack * stk, STACK_CHANGE condition)
{
    MY_ASSERT(stk, STK_ADDRESS_ERROR);

    switch (condition)
    {
        case UP:
        {
            stk->capacity *= 2;

            stk->data = (StackType*)realloc(stk->data, stk->capacity * sizeof(StackType));
            MY_ASSERT(stk->data, ADDRESS_ERROR);

            break;
        }

        case DOWN:
        {
            if (stk->size == 0)
            {
                stk->capacity = 1;
            }
            else
            {
                stk->capacity /= 4;
            }

            stk->data = (StackType*)realloc(stk->data, stk->capacity * sizeof(StackType));
            MY_ASSERT(stk->data, ADDRESS_ERROR);

            break;
        }

        default:
        {
            RED_TEXT("ERROR STACK CHANGE VALUE\n");
            return CHANGE_ERROR;

            break;
        }
    }

    return NO_ERROR;
}
