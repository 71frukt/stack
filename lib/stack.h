#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define DEBUG
#define CANARY_PROTECTION
#define HASH_PROTECTION

#include "stack_debug.h"

typedef int StackElem_t;

typedef int64_t StackID;
extern int StkError;

enum ResizeValue
{
    INCREASE,
    DECREASE
};

struct Stack_t
{
    ON_HASH (size_t hash);

    ON_CANARY (canary_t left_stack_canary);

    ON_DEBUG (
    int line_born_in;
    const char *file_born_in;
    FILE *logs_file;
    )

    ON_CANARY (
    canary_t *left_data_canary_ptr;
    canary_t *right_data_canary_ptr;
    )

    int size;
    int capacity;
    int start_capacity;
    
    StackElem_t *data;

    ON_CANARY (canary_t right_stack_canary);
};

const StackElem_t START_DATA_SIZE = 2;

StkAssertRes StackCtor (StackID *code, int start_capasity, const char* file, const int line);
StkAssertRes StackDtor (StackID code);
StkAssertRes StackPush (StackID code, StackElem_t value);
StkAssertRes StackPop  (StackID code, StackElem_t *stk_elem);
int PrintStackErr (int error);

#define STACK_CTOR(code, start_capacity) StackCtor(code, start_capacity, __FILE__, __LINE__)

#endif