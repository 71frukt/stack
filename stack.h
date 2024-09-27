#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEBUG
typedef int StackElem_t;

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

#define STACK_DUMP(stk)  StackDump (stk, __func__, __LINE__);

#ifdef DEBUG

#define INIT_DEBUG_STKVARS  .line_born_in = __LINE__, .file_born_in = __FILE__, .func_born_in = __func__

#define STACK_ASSERT(stk)                                   \
{                                                           \
    StkCondition condition = StackOK(stk);                  \
    if (condition != STK_OK)                                \
    {                                                       \
        fprintf(stderr, "%s", StackStrErr(condition));      \
        assert(0);                                          \
    }                                                       \
}

#define ON_DEBUG(...) __VA_ARGS__

#else 

#define INIT_DEBUG_STKVARS
#define STACK_ASSERT(stk)
#define ON_DEBUG(...)

#endif


struct Stack_t
{
    ON_DEBUG(int line_born_in);
    ON_DEBUG(const char *file_born_in);
    ON_DEBUG(const char *func_born_in);
    
    StackElem_t *data;
    int size;
    int capacity;
};

const StackElem_t START_STACK_SIZE = 1;

void StackInit       (Stack_t *stk);
void StackPush       (Stack_t *stk, StackElem_t value);
void StackPop        (Stack_t *stk, StackElem_t *stk_elem);
void StackResize     (Stack_t *stk, ResizeValue resize_val);
void StackDump       (Stack_t *stk, const char *func_name, const int line);
StackElem_t *StkDataRecalloc(Stack_t *stk_ptr, size_t new_data_size);
StkCondition StackOK (Stack_t *stk);
const char *StackStrErr(StkCondition err);

#endif