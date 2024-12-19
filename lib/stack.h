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

enum ResizeValue
{
    INCREASE,
    DECREASE
};

const StackElem_t START_DATA_SIZE = 2;

StkAssertRes StackCtor (StackID *code, int start_capasity, const char* file, const int line);
StkAssertRes StackDtor (StackID code);
StkAssertRes StackPush (StackID code, StackElem_t value);
StkAssertRes StackPop  (StackID code, StackElem_t *stk_elem);
int PrintStackErr (int error);

#define STACK_CTOR(code, start_capacity) StackCtor(code, start_capacity, __FILE__, __LINE__)

#endif