#include <stdio.h>

// #define DEBUG
//typedef int StackElem_t;
#include "stack.h"

int main()
{
    fprintf(stderr, "\n START \n");

    Stack_t stack = {INIT_DEBUG_STKVARS};
    StackInit(&stack);
    StackPush(&stack, 10);
    StackPush(&stack, 15);
    StackPush(&stack, 20);
    StackPush(&stack, 30);
    StackPush(&stack, 40);
    StackPush(&stack, 50);
    StackPush(&stack, 60);
 
    StackElem_t el = 0;
    StackPop(&stack, &el);
    StackPop(&stack, &el);
    StackPop(&stack, &el);

    StackDestruct(&stack);

    fprintf(stderr, "\n END \n"); 
}