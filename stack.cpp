#include <stdio.h>
#include <cstring>
#include "stack.h"
#include "stack_canary.h"

StkAssertRes StackInit(Stack_t *stk)
{
    StackElem_t *whole_data = (StackElem_t *) calloc(2, CANARY_SIZE);
    stk->data = (StackElem_t *) ((char *) whole_data + CANARY_SIZE);
    
    #ifdef DEBUG    
    stk->left_data_canary_ptr  = (canary_t *) ((char *)stk->data - CANARY_SIZE);
    stk->right_data_canary_ptr = (canary_t *) stk->data; 
    *(stk->left_data_canary_ptr)  = CANARY_VALUE;
    *(stk->right_data_canary_ptr) = CANARY_VALUE;
    #endif

    stk->size = 0;
    stk->capacity = 0;

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));
    return STK_ASSERT_OK;
}

StkAssertRes StackDestruct(Stack_t *stk)
{
    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    free(stk->data - CANARY_SIZE);
    stk->data = NULL;
    stk->capacity = 0;
    stk->size = 0;
   
    return STK_ASSERT_OK;
}

StkAssertRes StackResize(Stack_t *stk, ResizeValue resize_val)
{
    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    if (stk->capacity == 0)
    {
        stk->capacity = START_STACK_SIZE;
        memset(stk->data, 0, START_STACK_SIZE * sizeof(StackElem_t));
    }

    int new_capacity = (resize_val == INCREASE) ? stk->capacity * 2 : stk->capacity / 2;

    stk->data = (StackElem_t *) StackDataRecalloc(stk, new_capacity);
    stk->capacity = new_capacity;

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    return STK_ASSERT_OK;
} 

StackElem_t *StackDataRecalloc(Stack_t *stk, size_t new_capacity)
{
    fprintf(stderr, "\nRECALLOC!\n");
    fprintf(stk->logs_file, "\nRECALLOC!\n");

    //STACK_ASSERT(stk, NULL);
    //ON_DEBUG(STACK_DUMP(stk));
    size_t new_byte_capa = new_capacity * sizeof(StackElem_t);
    size_t residual = (CANARY_SIZE == 0) ? 0 : (CANARY_SIZE - new_byte_capa % CANARY_SIZE) % CANARY_SIZE;

    size_t new_data_bytesize = new_byte_capa + 2 * CANARY_SIZE + residual;
fprintf(stderr, "new_byte_capa = %lld; residual = %lld; new_data_bytesize = %lld\n", new_byte_capa, residual, new_data_bytesize);

    stk->data = (StackElem_t *) realloc((char *)stk->data - CANARY_SIZE, new_data_bytesize);
    stk->data = (StackElem_t *) ((char *) stk->data + CANARY_SIZE);

    stk->left_data_canary_ptr  = (canary_t*) ((char *)stk->data - CANARY_SIZE);
    stk->right_data_canary_ptr = (canary_t*) ((char *)stk->data + new_byte_capa + residual);

    *(stk->left_data_canary_ptr)  = CANARY_VALUE; 
    *(stk->right_data_canary_ptr) = CANARY_VALUE;

    memset(stk->data + stk->capacity, 0, new_byte_capa + residual - stk->capacity * sizeof(StackElem_t));

fprintf(stderr, "left canar : %lld  right :       %lld\n", *stk->left_data_canary_ptr, *stk->right_data_canary_ptr);
fprintf(stderr, "&left canar: %p    &right:       %p \n",  stk->left_data_canary_ptr, stk->right_data_canary_ptr);
fprintf(stderr, "data[0]    : %d    data[capa]  : %d\n", *stk->data, *stk->data + stk->capacity);
fprintf(stderr, "&data[0]   : %p    &data[capa] : %p\n\n", &stk->data[0], stk->data + stk->capacity);


    STACK_ASSERT(stk, NULL);
    ON_DEBUG(STACK_DUMP(stk));

    return stk->data;
}

StkAssertRes StackPush(Stack_t *stk, StackElem_t value)
{
    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    if (stk->size >= stk->capacity)
    {
        StackResize(stk, INCREASE);
    }

    stk->data[stk->size] = value; 
    // *( (StackElem_t *) ((char *) stk->data + stk->size * sizeof(StackElem_t) + CANARY_SIZE)) = value;
    // *DATA_EL_PTR(stk->data, stk->size) = value;
    stk->size++;

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));
    return STK_ASSERT_OK;  
}

StkAssertRes StackPop(Stack_t *stk, StackElem_t *stk_elem)
{
    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    if ((stk->size <= stk->capacity / 4) && (stk->capacity > START_STACK_SIZE))
    {
        StackResize(stk, DECREASE);
    }

    *stk_elem = stk->data[stk->size - 1];
    // *stk_elem = *DATA_EL_PTR(stk->data, stk->size - 1);
    stk->size--;

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));
    return STK_ASSERT_OK;
}

void StackDump(Stack_t *stk, const char *func_name, const int line)
{
    StkError stk_condition = StackOK(stk);

    fprintf(stk->logs_file, "STACK:\n");
    fprintf(stk->logs_file, "called from : func = %s, line = %d\n", func_name, line);
    fprintf(stk->logs_file, "born in: file = %s, func = %s, line = %d\n", stk->file_born_in, stk->func_born_in, stk->line_born_in);
    fprintf(stk->logs_file, "size     = %d\n", stk->size);
    fprintf(stk->logs_file, "capacity = %d\n", stk->capacity);
    fprintf(stk->logs_file, "left canar : %lld  \t\t\tright:\t\t  %lld\n", *stk->left_data_canary_ptr, *stk->right_data_canary_ptr);
    fprintf(stk->logs_file, "&left canar: %p    &right:       %p \n",  stk->left_data_canary_ptr, stk->right_data_canary_ptr);
    fprintf(stk->logs_file, "&data[0]   : %p    &data[capa] : %p\n\n", stk->data, stk->data + stk->capacity);

    fprintf(stk->logs_file, "data [%p]: { \n", stk);
    if (stk_condition != STK_PTR_DATA_ERR && stk_condition != STK_DATA_ERR && stk_condition != STK_SIZE_ERR)
        for (int i = 0; i < stk->capacity; i++)
            fprintf(stk->logs_file, "   data[%d] = %d \n", i, stk->data[i]);

    fprintf(stk->logs_file, "}\n\n");
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
        fprintf(stk->logs_file, "\n !!! %s in ", StackStrErr(condition));
        fprintf(stderr,         "\n !!! %s in ", StackStrErr(condition));
        return STK_ASSERT_ERR;                               
    } 

    return STK_ASSERT_OK;
}

