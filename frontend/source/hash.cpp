#include "../include/hash.h"
#include "../include/stack_config.h"

#ifdef HASH_PROTECTION

void HashIncrease (char* ptr, Hash_t* hash_ptr,
                   size_t begin_index, size_t border_index)
{
    for (size_t i = begin_index; i < border_index ; ++i)
    {
        *hash_ptr += (Hash_t) *(ptr + i);
    }
}

void HashDecrease (char* ptr, Hash_t* hash_ptr,
                   size_t begin_index, size_t border_index)
{
    for (size_t i = begin_index; i < border_index ; ++i)
    {
        *hash_ptr -= (Hash_t) *(ptr + i);
    }
}

ErrorType StackFindHash (Stack*  const stk,
                         Hash_t* const hash_ptr)
{
    STACK_VERIFY (stk);

    char* ptr = (char*) stk;
    *hash_ptr = 0;
    size_t begin  = 0;
    size_t border = sizeof (Stack) - sizeof (Hash_t);

    #ifdef CANARY_PROTECTION
        border -= sizeof (Canary_t);
    #endif

    HashIncrease (ptr, hash_ptr, begin, border);

    #ifdef CANARY_PROTECTION
        size_t canary_begin  = sizeof (Stack) - sizeof (Canary_t);
        size_t canary_border = sizeof (Stack);
        HashIncrease (ptr, hash_ptr, canary_begin, canary_border);
    #endif

    return stk->stack_err;
}

ErrorType StackDataFindHash (Stack*  const stk,
                             Hash_t* const hash_ptr)
{
    char* ptr = (char*) stk->data;
    HashIncrease (ptr, hash_ptr, 0, stk->data_capacity * sizeof (Elem_t));

    #ifdef CANARY_PROTECTION
        char* left_canary_ptr  = (ptr - sizeof(Canary_t));
        char* right_canary_ptr = (ptr + stk->data_capacity * sizeof (Elem_t));
        HashIncrease (left_canary_ptr,  hash_ptr, 0, sizeof (Canary_t));
        HashIncrease (right_canary_ptr, hash_ptr, 0, sizeof (Canary_t));
    #endif

    return stk->stack_err;
}

ErrorType StackHashError (Stack* const stk)
{
    assert (stk);

    Hash_t new_hash_value = 0;
    StackFindHash     (stk, &new_hash_value);
    StackDataFindHash (stk, &new_hash_value);

    if (stk->hash_value != new_hash_value)
    {
        stk->stack_err.STACK_HASH_DAMAGED = 1;
    }

    return stk->stack_err;
}

#endif
