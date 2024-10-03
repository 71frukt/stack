#include <stdio.h>
#include <assert.h>

// #define DEBUG
//typedef int StackElem_t;
int StkError = 0;

#include "stack.h"


int main()
{
    fprintf(stderr, "\n START \n");
    
    Stack_t stack = {INIT_DEBUG_STKVARS};
    
    StackCtor(&stack)      verified;
    StackPush(&stack, 10)  verified;
    StackPush(&stack, 15)  verified;
    StackPush(&stack, 20)  verified;
    StackPush(&stack, 300) verified;
    StackPush(&stack, 80)  verified;

    // stack.capacity = -8;
    // stack.hash = 9887786;
// 
    // for (int i = 0; i < 10; i++)
        // stack.data[stack.capacity + i] = 0;

    StackElem_t el = 0;
    StackPop(&stack, &el) verified;
    StackPop(&stack, &el) verified;
    StackPop(&stack, &el) verified;

    StackDtor(&stack);

    fprintf(stderr, "\n END \n"); 
}