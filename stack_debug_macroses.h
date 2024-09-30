#define STACK_DUMP(stk)  StackDump (stk, __func__, __LINE__);

#ifdef DEBUG

#define INIT_DEBUG_STKVARS                                  \
    .line_born_in = __LINE__,                               \
    .file_born_in = __FILE__,                               \
    .func_born_in = __func__,                               \
    .logs_file = fopen("logs.txt", "w")                     \

#define STACK_ASSERT(stk, returned_error)                   \
    if (StackAssert(stk) == STK_ASSERT_ERR)                 \
    {                                                       \
        STACK_DUMP(stk);                                    \
        return returned_error;                              \
    }                                                       \


#define ON_DEBUG(...) __VA_ARGS__

#else 

#define INIT_DEBUG_STKVARS
#define STACK_ASSERT(stk, returned_error)
#define ON_DEBUG(...)

#endif
