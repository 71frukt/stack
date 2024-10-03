#include <stdio.h>
#include <string.h>

#include "stack.h"
#include "stack_debug_macroses.h"

StkAssertRes StackAssert(Stack_t *stk, const char *file, int line, const char *func)
{
    StkError = StackOK(stk);
    /*
    if (condition != STK_OK)                          
    {                                                 
        fprintf(stderr,         "\n !!! %s in ", StackStrErr(condition));
        fprintf(stk->logs_file, "\n !!! %s in ", StackStrErr(condition));
        return STK_ASSERT_ERR;                               
    } 
    */                
    // size_t counter = 1;


    if (StkError == 0)
        return STK_ASSERT_OK;
    else 
    {
        fprintf(stderr, "myassertion failed in %s:\t%s:%d\t", func, file, line);
        return STK_ASSERT_ERR;
    }
}

int PrintStackErr(int error)
{   
    #define PRINT_ERROR(err, code)                      \
    if (err & code)                                     \
    {                                                   \
        fprintf(stderr, #code);                         \
        fprintf(stderr, " ");                           \
    }                                                   

    PRINT_ERROR(error, CANARY_ERR);
    PRINT_ERROR(error, HASH_ERR);
    PRINT_ERROR(error, STK_PTR_DATA_ERR);
    PRINT_ERROR(error, STK_DATA_ERR);
    PRINT_ERROR(error, STK_SIZE_ERR);
    PRINT_ERROR(error, STK_CAPACITY_ERR);

    #undef PUT_ERROR  

    printf("\n");
    
    return 0;  
    /*
    #define DESCR_(err)                         \
        case err: return #err; break;

    switch (err)
    {
    DESCR_(STK_OK);
    DESCR_(CANARY_ERR);
    DESCR_(HASH_ERR);
    DESCR_(STK_PTR_DATA_ERR);
    DESCR_(STK_DATA_ERR);
    DESCR_(STK_SIZE_ERR);
    DESCR_(STK_CAPACITY_ERR);
    
    default:
        break;
    }

    #undef DESCR_
    */
}

int StackOK(Stack_t *stk)
{
    #ifdef CANARY_PROTECTION
    if (stk->left_data_canary_ptr == NULL || stk->right_data_canary_ptr  == NULL \
            || *stk->left_data_canary_ptr != CANARY_VALUE || *stk->right_data_canary_ptr != CANARY_VALUE)
        StkError |= CANARY_ERR;
    #endif

    #ifdef HASH_PROTECTION
    size_t cur_hash = GetDataHash(stk);
    if (stk->hash != cur_hash)
        StkError |= HASH_ERR;
    #endif

    if (stk == NULL)
        StkError |= STK_PTR_DATA_ERR;

    if (stk->data == NULL)
        StkError |= STK_DATA_ERR;
    
    if (stk->size < 0 || stk->size > stk->capacity)
        StkError |= STK_SIZE_ERR;

    if (stk->capacity < 0)
        StkError |= STK_CAPACITY_ERR;

    return StkError;    
}

void StackDump(Stack_t *stk, const char *func_name, const int line)
{
    fprintf(stk->logs_file, "STACK:\n");
    fprintf(stk->logs_file, "called from : func = %s, line = %d\n", func_name, line);
    fprintf(stk->logs_file, "born in: file = %s, func = %s, line = %d\n", stk->file_born_in, stk->func_born_in, stk->line_born_in);
    fprintf(stk->logs_file, "size     = %d\n", stk->size);
    fprintf(stk->logs_file, "capacity = %d\n", stk->capacity);

    #ifdef CANARY_PROTECTION
    fprintf(stk->logs_file, "left canar : %lld  \t\t\tright:  %lld\n", *stk->left_data_canary_ptr, *stk->right_data_canary_ptr);
    fprintf(stk->logs_file, "&left canar: %p    &right:       %p \n",  stk->left_data_canary_ptr, stk->right_data_canary_ptr);
    fprintf(stk->logs_file, "&data[0]   : %p    &data[capa] : %p\n\n", stk->data, stk->data + stk->capacity);
    #endif

    #ifdef HASH_PROTECTION
    fprintf(stk->logs_file, "data hash  = %lld\n", stk->hash);
    #endif

    fprintf(stk->logs_file, "data [%p]: { \n", stk);
    for (int i = 0; i < stk->size; i++)
        fprintf(stk->logs_file, "   data[%d] = %d \n", i, stk->data[i]);

    fprintf(stk->logs_file, "}\n\n");
}

size_t GetDataHash(Stack_t *stk)
{
    size_t hash = 5381;

    for (int i = 0; i < stk->capacity; i++)
    {
        hash = hash * 33 + stk->data[i];
    }
    
    hash = hash * 33 +          stk->capacity;
    hash = hash * 33 + (size_t) stk->data;
    hash = hash * 33 +  strlen (stk->file_born_in);
    hash = hash * 33 +  strlen (stk->func_born_in);
    hash = hash * 33 +          stk->line_born_in;
    hash = hash * 33 + (size_t) stk->logs_file;
    hash = hash * 33 + (size_t) stk->left_data_canary_ptr;
    hash = hash * 33 + (size_t) stk->right_data_canary_ptr;
    hash = hash * 33 +          stk->size;

    return hash;
}