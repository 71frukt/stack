#include <stdio.h>

#define DEBUG
//typedef int StackElem_t;
#include "stack.h"

int main()
{
    fprintf(stderr, "\n START");


    Stack_t stack = {INIT_DEBUG_STKVARS};
    StackInit(&stack);
    StackPush(&stack, 100);
    StackPush(&stack, 150);
    StackPush(&stack, 200);
    StackPush(&stack, 300);
    StackPush(&stack, 400);
    StackPush(&stack, 500);
    StackPush(&stack, 600);
 
    StackElem_t el = 0;
    StackPop(&stack, &el);

    fprintf(stderr, "\n END \n");
}