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

void StackDestruct(Stack_t *stk)
{
    STACK_ASSERT(stk);

    free(stk->data);
    stk->data = NULL;
    stk->capacity = 0;
    stk->size = 0;
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

    stk->data = (StackElem_t *) StackDataRecalloc(stk, new_capacity);
    stk->capacity = new_capacity;

    ON_DEBUG(STACK_DUMP(stk));

    STACK_ASSERT(stk);
} 

StackElem_t *StackDataRecalloc(Stack_t *stk, size_t new_data_size)
{
    STACK_ASSERT(stk);
    
    StackElem_t *res_ptr = NULL;
    res_ptr = (StackElem_t *) realloc(stk->data, new_data_size * sizeof(StackElem_t));
    memset((StackElem_t *) res_ptr + stk->capacity, 0, (new_data_size - stk->capacity) * sizeof(StackElem_t));

    fprintf(stk->logs_file, "\nRECALLOC!\n");

    STACK_ASSERT(stk);

    return res_ptr;
}


void StackDump(Stack_t *stk, const char *func_name, const int line)
{
    StkError stk_condition = StackOK(stk);

    fprintf(stk->logs_file, "\n\nSTACK:\n");
    fprintf(stk->logs_file, "called from : func = %s, line = %d\n", func_name, line);
    fprintf(stk->logs_file, "born in: file = %s, func = %s, line = %d\n", stk->file_born_in, stk->func_born_in, stk->line_born_in);
    fprintf(stk->logs_file, "size     = %d\n", stk->size);
    fprintf(stk->logs_file, "capacity = %d\n", stk->capacity);
    fprintf(stk->logs_file, "data [%p]: { \n", stk);

    if (stk_condition != STK_PTR_DATA_ERR && stk_condition != STK_DATA_ERR && stk_condition != STK_SIZE_ERR)
        for (int i = 0; i < stk->size; i++)
            fprintf(stk->logs_file, "   data[%d] = %d \n", i, stk->data[i]);

    fprintf(stk->logs_file, "}");
}

StkError StackOK(Stack_t *stk)
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

const char *StackStrErr(StkError err)
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

StkAssertRes StackAssert(Stack_t *stk)
{
    StkError condition = StackOK(stk);                
    if (condition != STK_OK)                          
    {                                                 
        fprintf(stderr, "%s\n", StackStrErr(condition));
        return STK_ASSERT_ERR;                               
    } 

    return STK_ASSERT_OK;
}