#include <stdio.h>
#include <assert.h>

int StkError = 0;

#include "stack.h"

void BreakAfterDtor();

int main()
{
    fprintf(stderr, "\n START \n");

    StackID stk_id = 0; 
    STACK_CTOR(&stk_id, 0);

    //BreakAfterDtor();

    StackDtor(stk_id);

    fprintf(stderr, "\n END \n"); 
}

void BreakAfterDtor()
{
    StackID stk_id = 0; 
    STACK_CTOR(&stk_id, 0);

    Stack_t *bad_ptr = (Stack_t *) (stk_id ^ 0xDEADBEAF);

    StackPush(stk_id, 10);
    StackPush(stk_id, 15);
    StackPush(stk_id, 20);
    StackPush(stk_id, 300);
    StackPush(stk_id, 80);

    bad_ptr->data[3] = 4;

    StackDtor(stk_id);

    for (int i = 0; i < bad_ptr->capacity; i++)
    {
        printf("data[%d] = %d\n", i, bad_ptr->data[i]);
    }
}