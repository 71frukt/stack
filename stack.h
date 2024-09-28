#ifndef STACK_H
#define STACK_H

#define DEBUG
typedef int StackElem_t;

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "stack_debug_macroses.h"

enum ResizeValue
{
    INCREASE,
    DECREASE
};

enum StkCondition
{
    STK_OK,
    STK_PTR_DATA_ERR,
    STK_DATA_ERR,
    STK_SIZE_ERR,
    STK_CAPACITY_ERR
};

struct Stack_t
{
    ON_DEBUG(const int line_born_in);
    ON_DEBUG(const char *file_born_in);
    ON_DEBUG(const char *func_born_in);
    ON_DEBUG(FILE *logs_file);
    
    StackElem_t *data;
    int size;
    int capacity;
};

const StackElem_t START_STACK_SIZE = 1;

void StackInit      (Stack_t *stk);
void StackDestruct  (Stack_t *stk);
void StackPush      (Stack_t *stk, StackElem_t value);
void StackPop       (Stack_t *stk, StackElem_t *stk_elem);
void StackResize    (Stack_t *stk, ResizeValue resize_val);
void StackDump      (Stack_t *stk, const char *func_name, const int line);

StackElem_t *StkDataRecalloc(Stack_t *stk_ptr, size_t new_data_size);

StkCondition StackOK (Stack_t *stk);
const char *StackStrErr(StkCondition err);

#endif