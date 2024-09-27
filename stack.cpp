#include <stdio.h>
#include <cstring>
#include "stack.h"

void StackInit(Stack_t *stk)
{
    stk->data = (StackElem_t *) calloc(0, sizeof(StackElem_t));
    stk->size = 0;
    stk->capacity = 0;
    STACK_ASSERT(stk);
}

void StackPush(Stack_t *stk, StackElem_t value)
{
    STACK_ASSERT(stk);

    ON_DEBUG(STACK_DUMP(stk));

    if (stk->size >= stk->capacity)
    {
        StackResize(stk, INCREASE);
    }

    stk->data[stk->size] = value; 
    stk->size++;

    ON_DEBUG(STACK_DUMP(stk));

    STACK_ASSERT(stk);   
}

void StackPop(Stack_t *stk, StackElem_t *stk_elem)
{
    STACK_ASSERT(stk);
    ON_DEBUG(STACK_DUMP(stk));

    if ((stk->size <= stk->capacity / 4) && (stk->capacity > START_STACK_SIZE))
    {
        StackResize(stk, DECREASE);
    }

    *stk_elem = stk->data[stk->size - 1];
    stk->size--;
    STACK_ASSERT(stk);
}

void StackResize(Stack_t *stk, ResizeValue resize_val)
{
    STACK_ASSERT(stk);

    ON_DEBUG(STACK_DUMP(stk));

    if (stk->capacity == 0)
        stk->capacity = START_STACK_SIZE;

    int new_capacity = (resize_val == INCREASE) ? stk->capacity * 2 : stk->capacity / 2;

    stk->data = (StackElem_t *) StkDataRecalloc(stk, new_capacity);
    stk->capacity = new_capacity;

    ON_DEBUG(STACK_DUMP(stk));

    STACK_ASSERT(stk);
} 

StackElem_t *StkDataRecalloc(Stack_t *stk_ptr, size_t new_data_size)
{
    STACK_ASSERT(stk_ptr);
    
    StackElem_t *res_ptr = NULL;
    res_ptr = (StackElem_t *) realloc(stk_ptr->data, new_data_size * sizeof(StackElem_t));
    memset((StackElem_t *) res_ptr + stk_ptr->capacity, 0, (new_data_size - stk_ptr->capacity) * sizeof(StackElem_t));

    fprintf(stderr, "\nRECALLOC!\n");

    STACK_ASSERT(stk_ptr);

    return res_ptr;
}


void StackDump(Stack_t *stk, const char *func_name, const int line)
{
    fprintf(stderr, "\n\nSTACK:\n");
    fprintf(stderr, "called from : func = %s, line = %d\n", func_name, line);
    fprintf(stderr, "born in: file = %s, func = %s, line = %d\n", stk->file_born_in, stk->func_born_in, stk->line_born_in);
    fprintf(stderr, "size     = %d\n", stk->size);
    fprintf(stderr, "capacity = %d\n", stk->capacity);
    fprintf(stderr, "data [%p] \n", stk);
    fprintf(stderr, "data[size - 1] = %d\n", stk->data[stk->size - 1]);

    fprintf(stderr, "stack elements :  "); // TODO: поправить вывод
// data: {
    // data[0] = 100
    // data[1] = 150
    // 
// 
// }
    for (int i = 0; i < stk->size; i++)
    {
        fprintf(stderr, "%d ", stk->data[i]);
    }
}

StkCondition StackOK(Stack_t *stk)
{
    if (stk == NULL)
        return STK_PTR_DATA_ERR;

    if (stk->data == NULL)
        return STK_DATA_ERR;
    
    if (stk->size < 0 || stk->size > stk->capacity)
        return STK_SIZE_ERR;

    if (stk->capacity < 0)
        return STK_CAPACITY_ERR;

    return STK_OK;    
}

const char *StackStrErr(StkCondition err)
{
    #define DESCR_(err)                         \
        case err: return #err; break;

    switch (err)
    {
    DESCR_(STK_OK);
    DESCR_(STK_PTR_DATA_ERR);
    DESCR_(STK_DATA_ERR);
    DESCR_(STK_SIZE_ERR);
    DESCR_(STK_CAPACITY_ERR);
    
    default:
        break;
    }

    return NULL;

    #undef DESCR_
}