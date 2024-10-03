#include <stdio.h>
#include <assert.h>

// #define DEBUG
//typedef int StackElem_t;
int StkError = 0;

#include "stack.h"


int main()
{
    fprintf(stderr, "\n START \n");
    
    StackID stk_id = 0; 
    // Stack_t stack = {INIT_DEBUG_STKVARS};
    
    STACK_CTOR(&stk_id)    verified;
    StackPush(stk_id, 10)  verified;
    StackPush(stk_id, 15)  verified;
    StackPush(stk_id, 20)  verified;
    StackPush(stk_id, 300) verified;
    StackPush(stk_id, 80)  verified;

    // size_t *ptr;
    // for (int i = 0; i < 1000; i++)
        // ptr[i] = 0;

    StackElem_t el = 0;
    StackPop(stk_id, &el) verified;
    StackPop(stk_id, &el) verified;
    StackPop(stk_id, &el) verified;

    StackDtor(stk_id);

    fprintf(stderr, "\n END \n"); 
}