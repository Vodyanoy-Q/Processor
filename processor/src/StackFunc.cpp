#include <stdio.h>
#include <stdlib.h>

#include "../inc/Stack.h"
#include "../inc/colors.h"

void DataDump(Stack * stk)
{
    for (unsigned int i = 0; i < stk->capacity; i++)
    {
        if (i < stk->size)
        {
            if (i == stk->size - 1)
            {
                printf("%s%5lg %s", RED, stk->data[i], RESET_COLOR);
                continue;
            }
            printf("%s%5lg %s", CYAN, stk->data[i], RESET_COLOR);
        }
        else
        {
            printf("%s%5lg %s", GREEN, stk->data[i], RESET_COLOR);
        }
    }
}

Stack * StackCtor(size_t capacity)
{
    Stack * stk = (Stack*)calloc(1, sizeof(Stack));
    MY_ASSERT(stk);

    stk->size = 0;
    stk->capacity = capacity;

    stk->data = (StackType*)calloc(stk->capacity, sizeof(StackType));
    MY_ASSERT(stk);

    return stk;
}

void StackDtor(Stack ** stk)
{
    (*stk)->size = 0;
    (*stk)->capacity = 0;

    free((*stk)->data);
    free(*stk);

    *stk = NULL;
}

int StackPush(Stack * stk, StackType value)
{
    MY_ASSERT(stk);

    if (stk->size == stk->capacity)
    {
        StackChange(stk, UP);
    }

    stk->data[stk->size] = value;

    stk->size += 1;

    return 0;
}

void StackDump(Stack * stk)
{
    MY_ASSERT(stk);

    printf("==================================================\n");

    for (size_t i = 0; i < stk->capacity; i++)
    {
        printf("%5lg ", stk->data[i]);
    }

    printf("\n==================================================\n");
}

StackType StackPop(Stack * stk)
{
    MY_ASSERT(stk);

    StackType var = 0;

    if (stk->size == 0)
    {
        printf("There is no values in the Stack. Push values in the Stack and try again\n");
    }
    else
    {
        var = stk->data[stk->size - 1];
        stk->data[stk->size - 1] = 0;

        stk->size -= 1;

        if (stk->size == 0)
        {
            stk->capacity = 1;

            StackChange(stk, DOWN);
        }

        else if (stk->size > 0 && stk->capacity / 4 == stk->size)
        {
            StackChange(stk, DOWN);
        }
    }

    return var;
}

void StackChange(Stack * stk, STACK_CHANGE condition)
{
    MY_ASSERT(stk);

    switch (condition)
    {
        case UP:
        {
            stk->capacity *= 2;

            stk->data = (StackType*)realloc(stk->data, stk->capacity * sizeof(StackType));
            MY_ASSERT(stk->data);

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
            MY_ASSERT(stk->data);

            break;
        }

        default:
        {
            printf("ERROR STACK CHANGE VALUE\n");
            exit(CHANGE_ERROR);

            break;
        }
    }
}
