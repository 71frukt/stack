#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEBUG
#define CANARY_PROTECTION
#define HASH_PROTECTION

extern int StkError;

typedef int StackElem_t;

#include "stack_debug.h"

enum ResizeValue
{
    INCREASE,
    DECREASE
};

struct Stack_t
{
    #ifdef DEBUG
    const int line_born_in;
    const char *file_born_in;
    const char *func_born_in;
    FILE *logs_file;
    #endif

    #ifdef CANARY_PROTECTION
    canary_t *left_data_canary_ptr;
    canary_t *right_data_canary_ptr;
    #endif

    #ifdef HASH_PROTECTION
    size_t hash;
    #endif

    int size;
    int capacity;
    StackElem_t *data;
};

const StackElem_t START_STACK_SIZE = 2;

StkAssertRes StackCtor (Stack_t *stk);
StkAssertRes StackDtor (Stack_t *stk);
StkAssertRes StackPush (Stack_t *stk, StackElem_t value);
StkAssertRes StackPop  (Stack_t *stk, StackElem_t *stk_elem);

int          PrintStackErr (int error);
#endif