#include <stdio.h>
#include <cstring>

#include "stack.h"
#include "stack_debug.h"

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

static int StkError = 0;

static uint64_t POISON         = 0xDEB41C;
static uint64_t STK_ENCODE_KEY = 0xDEADBEAF;

static StkAssertRes StackResize       (Stack_t *stk, ResizeValue resize_val);
static StackElem_t *StackDataRecalloc (Stack_t *stk, size_t new_capacity);

static StkAssertRes StackAssert       (Stack_t *stk, const char *file, const int line);
static void         StackDump         (Stack_t *stk, const char *file, const int line);
static int          StackOK           (Stack_t *stk);
static size_t       GetStackHash      (Stack_t *stk);
static size_t       GetDataHash       (Stack_t *stk);

static StackID      StackEncode       (Stack_t *stk);
static Stack_t     *StackDecode       (StackID code);


StkAssertRes StackCtor(StackID *code, int start_capacity, const char* file, const int line)
{
    Stack_t *stk = (Stack_t *) calloc(1, sizeof(Stack_t));

    ON_DEBUG (                                                                                  \
        stk->file_born_in = file ,                                                              \
        stk->line_born_in = line,                                                               \
        stk->logs_file = fopen("logs.txt", "w")                                                 \
    );

    ON_DEBUG (setvbuf(stk->logs_file, NULL, _IONBF, 0));

    StackElem_t *whole_data = (StackElem_t *) calloc(2, CANARY_SIZE);
    stk->data = (StackElem_t *) ((char *) whole_data + CANARY_SIZE);
    
    stk->size = 0;
    stk->capacity = 0;
    stk->start_capacity = start_capacity;

    ON_CANARY (    
        stk->left_stack_canary  = CANARY_VALUE;
        stk->right_stack_canary = CANARY_VALUE;

        stk->left_data_canary_ptr  = (canary_t *) ((char *)stk->data - CANARY_SIZE);
        stk->right_data_canary_ptr = (canary_t *) stk->data; 
        *(stk->left_data_canary_ptr)  = CANARY_VALUE;
        *(stk->right_data_canary_ptr) = CANARY_VALUE;
    )

    ON_HASH (
        stk->hash = GetStackHash(stk);
    )

    stk->data = (StackElem_t *) StackDataRecalloc(stk, start_capacity);    

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    *code = StackEncode(stk);

    return STK_ASSERT_OK;
}

StkAssertRes StackDtor(StackID code)
{
    Stack_t *stk = StackDecode(code);

    STACK_ASSERT(stk, STK_ASSERT_ERR);

    if (stk == NULL)
        return STK_ASSERT_ERR;

    if (stk->data != NULL)
    {
        free((char *)stk->data - CANARY_SIZE);
        stk->data = NULL;
    }
    
    stk->capacity = 0;
    stk->size = 0;

    ON_DEBUG(fclose(stk->logs_file));

    ON_DEBUG(STACK_DUMP(stk));

    free(stk);

    return STK_ASSERT_OK;
}

StkAssertRes StackPush(StackID code, StackElem_t value)
{
    Stack_t *stk = StackDecode(code);

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    if (stk->size >= stk->capacity)
    {
        StackResize(stk, INCREASE);
    }

    stk->data[stk->size] = value; 
    stk->size++;

    ON_HASH(stk->hash = GetStackHash(stk));

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));
    return STK_ASSERT_OK;  
}

StkAssertRes StackPop(StackID code, StackElem_t *stk_elem)
{    
    Stack_t *stk = StackDecode(code);

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    if ((stk->size <= stk->capacity / 4) && (stk->capacity > stk->start_capacity))
    {
        ON_DEBUG(fprintf(stk->logs_file, "\n\nPOP_RECALLOC!\n"));
        StackResize(stk, DECREASE);
    }

    if (stk->size <= 0)
    {
        fprintf(stderr, "SIZE <= 0! \n");
        return STK_ASSERT_ERR;
    }

    StackElem_t tmp = 0;
    tmp = stk->data[stk->size - 1];
    ON_DEBUG(stk->data[stk->size - 1] = (StackElem_t) POISON);

    stk->size--;

    ON_HASH(stk->hash = GetStackHash(stk));
    *stk_elem = tmp;

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    return STK_ASSERT_OK;
}

static StackID StackEncode(Stack_t *stk)
{
    return (StackID)stk ^ STK_ENCODE_KEY;   
}

static Stack_t *StackDecode(StackID code)
{
    return (Stack_t *)(code ^ STK_ENCODE_KEY);
}

static StkAssertRes StackResize(Stack_t *stk, ResizeValue resize_val)
{
    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    if (stk->capacity == 0)
    {
        stk->data = (StackElem_t *) StackDataRecalloc(stk, START_DATA_SIZE);
        stk->start_capacity = START_DATA_SIZE;
        ON_HASH(stk->hash = GetStackHash(stk));
        STACK_ASSERT(stk, STK_ASSERT_ERR);
        return STK_ASSERT_OK;
    }

    int new_capacity = (resize_val == INCREASE) ? stk->capacity * 2 : stk->capacity / 2;
    stk->data = (StackElem_t *) StackDataRecalloc(stk, new_capacity);

    STACK_ASSERT(stk, STK_ASSERT_ERR);
    ON_DEBUG(STACK_DUMP(stk));

    return STK_ASSERT_OK;
} 

static StackElem_t *StackDataRecalloc(Stack_t *stk, size_t new_capacity)
{
    ON_DEBUG(fprintf(stk->logs_file, "\nRECALLOC!\n"));

    size_t new_byte_capacity = new_capacity * sizeof(StackElem_t);
    ON_CANARY( size_t residual = (CANARY_SIZE - new_byte_capacity % CANARY_SIZE) % CANARY_SIZE );

    size_t new_data_bytesize = new_byte_capacity  ON_CANARY(+ 2 * CANARY_SIZE + residual);

    // stk->data = (StackElem_t *) realloc((char *)stk->data - CANARY_SIZE, new_data_bytesize);
    char * tmp_ptr = (char *) realloc((char *)stk->data  ON_CANARY( - CANARY_SIZE ), new_data_bytesize);
    stk->data = (StackElem_t *) (tmp_ptr ON_CANARY(+ CANARY_SIZE));

    #ifdef CANARY_PROTECTION
    stk->left_data_canary_ptr  = (canary_t*) ((char *)stk->data ON_CANARY(- CANARY_SIZE));
    stk->right_data_canary_ptr = (canary_t*) ((char *)stk->data + new_byte_capacity ON_CANARY(+ residual));

    *(stk->left_data_canary_ptr)  = CANARY_VALUE; 
    *(stk->right_data_canary_ptr) = CANARY_VALUE;
    #endif

    ON_DEBUG (
        if (new_capacity > (size_t) stk->capacity)
        {
            // memset(stk->data + stk->capacity, (StackElem_t) POISON, new_byte_capacity ON_CANARY(+ residual) - stk->capacity * sizeof(StackElem_t)); // 
            for (int i = stk->capacity; i < (int) new_capacity; i++)
            {
                stk->data[i] = (StackElem_t) POISON;
            }
        }
    )

    stk->capacity = (int) new_capacity;

    ON_HASH(stk->hash = GetStackHash(stk));

    STACK_ASSERT(stk, NULL);
    ON_DEBUG(STACK_DUMP(stk));

    return stk->data;
}

static StkAssertRes StackAssert(Stack_t *stk, const char *file, const int line)
{
    StkError = StackOK(stk);

    if (StkError == 0)
        return STK_ASSERT_OK;
    else 
    {
        fprintf(stderr, "myassertion failed in\t%s:%d\nErrors:\t", file, line);
        PrintStackErr(StkError);
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
    
    PRINT_ERROR (error, STK_PTR_DATA_ERR);
    PRINT_ERROR (error, STACK_CANARY_ERR);
    PRINT_ERROR (error, DATA_CANARY_ERR);
    PRINT_ERROR (error, HASH_ERR);
    PRINT_ERROR (error, STK_DATA_ERR);
    PRINT_ERROR (error, STK_SIZE_ERR);
    PRINT_ERROR (error, STK_CAPACITY_ERR);

    #undef PRINT_ERROR  

    printf("\n");
    
    return 0;  
}

static int StackOK(Stack_t *stk)
{
    if (stk == NULL)
    {
        StkError |= STK_PTR_DATA_ERR;
        return StkError;
    }

    if (stk->data == NULL)
        StkError |= STK_DATA_ERR;

    #ifdef CANARY_PROTECTION
    if (stk->left_stack_canary != CANARY_VALUE || stk->right_stack_canary != CANARY_VALUE)
        StkError |= STACK_CANARY_ERR;
    
    if (stk->left_data_canary_ptr == NULL || stk->right_data_canary_ptr  == NULL \
            || *stk->left_data_canary_ptr != CANARY_VALUE || *stk->right_data_canary_ptr != CANARY_VALUE)
        StkError |= DATA_CANARY_ERR;
    #endif

    #ifdef HASH_PROTECTION
    size_t cur_hash = GetStackHash(stk);
    if (stk->hash != cur_hash)
        StkError |= HASH_ERR;
    #endif
    
    if (stk->size < 0 || stk->size > stk->capacity)
        StkError |= STK_SIZE_ERR;

    if (stk->capacity < 0)
        StkError |= STK_CAPACITY_ERR;

    return StkError;    
}

static void StackDump(Stack_t *stk, const char *file, const int line)
{
    ON_DEBUG (
    fprintf(stk->logs_file, "Stack_t[%p] at %s:%d born at %s:%d:\n", stk, file, line, stk->file_born_in, stk->line_born_in);
    fprintf(stk->logs_file, "{\n");
    fprintf(stk->logs_file, "\tsize     = %d\n", stk->size);
    fprintf(stk->logs_file, "\tcapacity = %d\n\n", stk->capacity);

    #ifdef CANARY_PROTECTION
    fprintf(stk->logs_file, "\tleft canar : %lld\t\t\tright canar:  %lld\n", *stk->left_data_canary_ptr, *stk->right_data_canary_ptr);
    fprintf(stk->logs_file, "\t&left canar: %p\t&right:       %p \n",  stk->left_data_canary_ptr, stk->right_data_canary_ptr);
    fprintf(stk->logs_file, "\t&data[0]   : %p\t&data[capa] : %p\n\n", stk->data, stk->data + stk->capacity);
    #endif

    #ifdef HASH_PROTECTION
    fprintf(stk->logs_file, "\tdata hash  = %lld\n\n", stk->hash);
    #endif

    fprintf(stk->logs_file, "\tdata [%p]: { \n", stk);
    for (int i = 0; i < stk->capacity; i++)
    {
        if (stk->data[i] == (StackElem_t) POISON)
        {
            fprintf(stk->logs_file, "\t\tdata[%d] = -POISON-\n", i);
            continue;
        }

        fprintf(stk->logs_file, "\t\tdata[%d] = %d \n", i, stk->data[i]);
    }

    fprintf(stk->logs_file, "\t}\n");
    fprintf(stk->logs_file, "}\n\n");

    );
}

static size_t GetStackHash(Stack_t *stk)
{
    size_t hash = 5381;
    hash = hash * 33 + GetDataHash(stk);

    size_t hash_size = sizeof(Stack_t);
    
    for (size_t byte_num = 0  ON_HASH(+ sizeof(size_t)); byte_num < hash_size; byte_num++)
    {
        hash = hash * 33 + *((char *)stk + byte_num);
    }

    /*
    for (int i = 0; i < stk->capacity; i++)
    {
        hash = hash * 33 + stk->data[i];
    }
    
    hash = hash * 33 +          stk->capacity;
    hash = hash * 33 + (size_t) stk->data;
    ON_DEBUG (
        hash = hash * 33 +  strlen (stk->file_born_in);
        hash = hash * 33 +          stk->line_born_in;
        hash = hash * 33 + (size_t) stk->logs_file;
    );

    ON_CANARY (
        hash = hash * 33 + (size_t) stk->left_data_canary_ptr;
        hash = hash * 33 + (size_t) stk->right_data_canary_ptr;
    );

    hash = hash * 33 +          stk->size;
    */

    return hash;
}

static size_t GetDataHash(Stack_t *stk)
{
    size_t hash = 5381;

    for (int i = 0; i < stk->capacity; i++)
    {
        hash = hash * 33 + stk->data[i];
    }

    return hash;
}