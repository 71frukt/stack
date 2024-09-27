#include <stdio.h>

#define DEBUG
//typedef int StackElem_t;
#include "stack.h"

int main()
{
    Stack_t stack = {INIT_DEBUG_STKVARS};
    StackInit(&stack);

fprintf(stderr, "After init: \n");
    STACK_DUMP(&stack);

    StackPush(&stack, 100);

fprintf(stderr, "\n\nAfter push(100)");
    STACK_DUMP(&stack);

    StackPush(&stack, 150);

fprintf(stderr, "\n\nAfter push(150)");
    STACK_DUMP(&stack);

    StackPush(&stack, 200);
    StackPush(&stack, 300);
    StackPush(&stack, 400);
    StackPush(&stack, 500);
    StackPush(&stack, 600);

fprintf(stderr, "\n\nAfter push(200, 300, 400, 500, 600)");
    STACK_DUMP(&stack);
    
    StackElem_t el = 0;
    StackPop(&stack, &el);
fprintf(stdout,"\nPOPED_EL = %d\n", el);

fprintf(stderr, "\n\nAfter pop");
    STACK_DUMP(&stack); 

    fprintf(stderr, "\n END");
}