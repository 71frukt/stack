#ifndef STACK_DEBUG_MACROSES
#define STACK_DEBUG_MACROSES

#include "stack.h"

enum StkAssertRes
{
    STK_ASSERT_OK  = 1,
    STK_ASSERT_ERR = 0
};

enum StkErrorEnum
{
    STK_OK = 0,

    STACK_CANARY_ERR = (1 << 0),
    DATA_CANARY_ERR  = (1 << 1),
    HASH_ERR         = (1 << 2),
    STK_PTR_DATA_ERR = (1 << 3),
    STK_DATA_ERR     = (1 << 4),
    STK_SIZE_ERR     = (1 << 5),
    STK_CAPACITY_ERR = (1 << 6)
};


#define STACK_DUMP(stk)  StackDump(stk, __FILE__, __LINE__)


#ifdef DEBUG

#define ON_DEBUG(...)  __VA_ARGS__

#define stk_asserted  || PrintStackErr(StkError)

#define INIT_DEBUG_STKVARS                                                                  \
    .line_born_in = __LINE__,                                                               \
    .file_born_in = __FILE__,                                                               \
    .func_born_in = __func__,                                                               \
    .logs_file = fopen("txts/logs.txt", "w")                                                     \

#define STACK_ASSERT(stk, returned_error)                                                   \
    if (StackAssert(stk, __FILE__, __LINE__) == STK_ASSERT_ERR)                             \
    {                                                                                       \
        STACK_DUMP(stk);                                                                    \
        return returned_error;                                                              \
    }                                                                                       \


#else 
#define stk_verified
#define INIT_DEBUG_STKVARS
#define STACK_ASSERT(stk, returned_error)
#define ON_DEBUG(...)
#endif


typedef long long int canary_t;
const canary_t CANARY_VALUE = 0xDEADBEEF; 


#ifdef CANARY_PROTECTION

#define ON_CANARY(...)  __VA_ARGS__
#define CANARY_SIZE  sizeof(canary_t)

#else
#define ON_CANARY(...)
#define CANARY_SIZE  0
#endif


#ifdef HASH_PROTECTION

#define ON_HASH(...)  __VA_ARGS__

#else
#define ON_HASH(...)
#endif

#endif