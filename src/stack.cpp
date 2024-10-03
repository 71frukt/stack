#include <stdio.h>
#include <cstring>

#include "stack.h"
#include "stack_debug.h"

static StkAssertRes StackResize       (Stack_t *stk, ResizeValue resize_val);
static StackElem_t *StackDataRecalloc (Stack_t *stk, int new_capacity);

static StkAssertRes StackAssert   (Stack_t *stk, const char *file, int line, const char *func);
static int          StackOK       (Stack_t *stk);
static void         StackDump     (Stack_t *stk, const char *func_name, const int line);
static size_t       GetDataHash   (Stack_t *stk);


StkAssertRes StackCtor(Stack_t *stk)
{
    StackElem_t *whole_data = (StackElem_t *) calloc(2, CANARY_SIZE);
    stk->data = (StackElem_t *) ((char *) whole_data + CANARY_SIZE);
    
    stk->size = 0;
    stk->capacity = 0;

    ON_CANARY (    
    stk->left_data_canary_ptr  = (canary_t *) ((char *)stk->data - CANARY_SIZE);
    stk->right_data_canary_ptr = (canary_t *) stk->data; 
    *(stk->left_data_canary_ptr)  = CANARY_VALUE;
    *(stk->right_data_canary_ptr) = CANARY_VALUE;
    )

    ON_HASH (
    stk->hash = GetDataHash(stk);
    fprintf(stderr, "hash = %lld\n", stk->hash);
    )

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));
    return STK_ASSERT_OK;
}

StkAssertRes StackDtor(Stack_t *stk)
{
    free((char *)stk->data - CANARY_SIZE);
    stk->data = NULL;
    stk->capacity = 0;
    stk->size = 0;

    ON_DEBUG(fclose(stk->logs_file));

    ON_DEBUG(STACK_DUMP(stk));

    return STK_ASSERT_OK;
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
    stk->size++;

    ON_HASH(stk->hash = GetDataHash(stk));

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
fprintf(stk->logs_file, "\n\nPOP_RECALLOC!\n");
        StackResize(stk, DECREASE);
    }

    if (stk->size <= 0)
    {
        fprintf(stderr, "SIZE <= 0! \n");
        return STK_ASSERT_ERR;
    }

    *stk_elem = stk->data[stk->size - 1];
    stk->size--;

    ON_HASH(stk->hash = GetDataHash(stk));

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));
    return STK_ASSERT_OK;
}

static StkAssertRes StackResize(Stack_t *stk, ResizeValue resize_val)
{
    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    if (stk->capacity == 0)
    {
        stk->data = (StackElem_t *) StackDataRecalloc(stk, START_STACK_SIZE);
        STACK_ASSERT(stk, STK_ASSERT_ERR);
        return STK_ASSERT_OK;
    }

    int new_capacity = (resize_val == INCREASE) ? stk->capacity * 2 : stk->capacity / 2;
    stk->data = (StackElem_t *) StackDataRecalloc(stk, new_capacity);

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    return STK_ASSERT_OK;
} 

static StackElem_t *StackDataRecalloc(Stack_t *stk, int new_capacity)
{
fprintf(stderr,         "\nRECALLOC!\n");
fprintf(stderr, "nwe_capa = %d, old_capa = %d \n", new_capacity, stk->capacity);
    ON_DEBUG(fprintf(stk->logs_file, "\nRECALLOC!\n"));

    size_t new_byte_capa = new_capacity * sizeof(StackElem_t);
    ON_CANARY( size_t residual = (CANARY_SIZE - new_byte_capa % CANARY_SIZE) % CANARY_SIZE );

    size_t new_data_bytesize = new_byte_capa  ON_CANARY(+ 2 * CANARY_SIZE + residual);
fprintf(stderr, "new_byte_capa = %lld; residual = %lld; new_data_bytesize = %lld\n", new_byte_capa, residual, new_data_bytesize);

    // stk->data = (StackElem_t *) realloc((char *)stk->data - CANARY_SIZE, new_data_bytesize);
    char * tmp_ptr = (char *) realloc((char *)stk->data  ON_CANARY( - CANARY_SIZE ), new_data_bytesize);
    stk->data = (StackElem_t *) (tmp_ptr ON_CANARY(+ CANARY_SIZE));

    #ifdef CANARY_PROTECTION
    stk->left_data_canary_ptr  = (canary_t*) ((char *)stk->data ON_CANARY(- CANARY_SIZE));
    stk->right_data_canary_ptr = (canary_t*) ((char *)stk->data + new_byte_capa ON_CANARY(+ residual));

    *(stk->left_data_canary_ptr)  = CANARY_VALUE; 
    *(stk->right_data_canary_ptr) = CANARY_VALUE;
    #endif

    if (new_capacity > stk->capacity)
        memset(stk->data + stk->capacity, 0, new_byte_capa + residual - stk->capacity * sizeof(StackElem_t));

    stk->capacity = new_capacity;

    ON_HASH(stk->hash = GetDataHash(stk));

fprintf(stderr, "hash = %lld\n", stk->hash);

fprintf(stderr, "left canar : %lld  right :       %lld\n", *stk->left_data_canary_ptr, *stk->right_data_canary_ptr);
fprintf(stderr, "&left canar: %p    &right:       %p\n",    stk->left_data_canary_ptr, stk->right_data_canary_ptr);
fprintf(stderr, "data[0]    : %d    data[capa]  : %d\n",   *stk->data, *stk->data + stk->capacity);
fprintf(stderr, "&data[0]   : %p    &data[capa] : %p\n\n", &stk->data[0], stk->data + stk->capacity);

    STACK_ASSERT(stk, NULL);
    ON_DEBUG(STACK_DUMP(stk));

    return stk->data;
}

static StkAssertRes StackAssert(Stack_t *stk, const char *file, int line, const char *func)
{
    StkError = StackOK(stk);

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
}

static int StackOK(Stack_t *stk)
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

static void StackDump(Stack_t *stk, const char *func_name, const int line)
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

static size_t GetDataHash(Stack_t *stk)
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