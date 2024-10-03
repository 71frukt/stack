#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEBUG
#define CANARY_PROTECTION
#define HASH_PROTECTION

#include "stack_debug.h"

typedef int StackElem_t;

typedef size_t StackID;
extern int StkError;

enum ResizeValue
{
    INCREASE,
    DECREASE
};

struct Stack_t
{
    #ifdef DEBUG
    int line_born_in;
    const char *file_born_in;
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

StkAssertRes StackCtor (StackID *code, const char* file, const int line);
StkAssertRes StackDtor (StackID code);
StkAssertRes StackPush (StackID code, StackElem_t value);
StkAssertRes StackPop  (StackID code, StackElem_t *stk_elem);
int PrintStackErr (int error);

#define STACK_CTOR(code) StackCtor(code, __FILE__, __LINE__)

#endif