#include <stdio.h>

// #define DEBUG
//typedef int StackElem_t;

#define DEBUG
#define CANARY_PROTECTION

#include "stack.h"

int main()
{
    fprintf(stderr, "\n START \n");
    
    Stack_t stack = {INIT_DEBUG_STKVARS};
    StackInit(&stack);
    StackPush(&stack, 10);
    StackPush(&stack, 15);
    StackPush(&stack, 20);

    // for (int i = 0; i < 10; i++)
        // stack.data[stack.capacity + i] = 0;

    StackElem_t el = 0;
    StackPop(&stack, &el);
    StackPop(&stack, &el);
    StackPop(&stack, &el);

    StackDestruct(&stack);

    fprintf(stderr, "\n END \n"); 
}