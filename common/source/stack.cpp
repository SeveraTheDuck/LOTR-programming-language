#include "../include/stack.h"

#ifdef HASH_PROTECTION
    #include "../include/hash.h"
#endif

ErrorType StackCtor (Stack* const stk,
                     #ifdef _DEBUG
                         STACK_CTOR_RECIVE_INFO
                     #endif
                     size_t stack_capacity)
{
    assert (stk);

    stk->data_size     = 0;
    stk->data_capacity = stack_capacity;

    #ifdef _DEBUG
        stk->init_name = init_name;
        stk->init_line = init_line;
        stk->init_file = init_file;
        stk->init_func = init_func;
    #endif

    memset (&stk->stack_err, 0, sizeof (ErrorType));

    #ifdef CANARY_PROTECTION
        stk->left_canary  = CANARY_VALUE;
        stk->right_canary = CANARY_VALUE;
    #endif

    #ifdef HASH_PROTECTION
        stk->hash_value = 0;
    #endif

    StackDataAlloc (stk, nullptr);

    STACK_VERIFY (stk);

    return stk->stack_err;
}

ErrorType StackDtor (Stack* stk)
{
    STACK_VERIFY (stk);

    #ifdef CANARY_PROTECTION
        stk->data = (Elem_t*) ((Canary_t*)(void*) stk->data - 1);
    #endif

    free (stk->data);
    stk->data          = nullptr;

    stk->data_size     = 0;
    stk->data_capacity = 0;

    #ifdef _DEBUG
        stk->init_line = 0;
        stk->init_name = nullptr;
        stk->init_file = nullptr;
        stk->init_func = nullptr;
    #endif
    stk->stack_err     = {};

    #ifdef CANARY_PROTECTION
        stk->left_canary  = 0;
        stk->right_canary = 0;
    #endif

    #ifdef HASH_PROTECTION
        stk->hash_value   = 0;
    #endif

    stk = nullptr;
    ErrorType stack_dtor_err = {};

    return stack_dtor_err;
}

ErrorType StackDataAlloc (Stack* const stk, Elem_t* const allocated_memory)
{
    assert (stk);

    /// Data align in case of sizeof (Elem_t) != sizeof (Canary_t).
    #ifdef CANARY_PROTECTION
        if ((stk->data_capacity *  sizeof (Elem_t))  % sizeof (Canary_t))
        {
            stk->data_capacity += (sizeof (Canary_t) - (stk->data_capacity *
                                   sizeof (Elem_t))  % sizeof (Canary_t)) /
                                   sizeof (Elem_t);
        }
    #endif

    size_t new_size = stk->data_capacity * sizeof (Elem_t);

    #ifdef CANARY_PROTECTION
        new_size += 2 * sizeof (Canary_t);
    #endif

    /// Allocation itself.
    stk->data = (Elem_t*) realloc (allocated_memory, new_size);
    if (StackDataPtrError (stk))
    {
        return stk->stack_err;
    }

    #ifdef CANARY_PROTECTION
        stk->data = (Elem_t*) ((Canary_t*)(void*) stk->data + 1);
        FillCanary (stk);
    #endif

    for (size_t i = stk->data_size; i < stk->data_capacity; ++i)
    {
        *(stk->data + i) = POISON;
    }

    #ifdef HASH_PROTECTION
        StackFindHash     (stk, &stk->hash_value);
        StackDataFindHash (stk, &stk->hash_value);
    #endif

    STACK_VERIFY (stk);

    return stk->stack_err;
}

#ifdef CANARY_PROTECTION
ErrorType FillCanary (Stack* const stk)
{
    *((Canary_t*)(void*)  stk->data - 1)                  = CANARY_VALUE;
    *( Canary_t*)(void*) (stk->data + stk->data_capacity) = CANARY_VALUE;

    return stk->stack_err;
}
#endif

ErrorType StackPush (Stack* const stk, const Elem_t value)
{
    STACK_VERIFY (stk);

    if (stk->data_size == stk->data_capacity)
    {
        stk->data_capacity *= RESIZE_MULTIPLIER;

        #ifdef CANARY_PROTECTION
            stk->data = (Elem_t*) ((Canary_t*)(void*) stk->data - 1);
        #endif

        StackDataAlloc(stk, (Elem_t*) stk->data);
    }

    #ifdef HASH_PROTECTION
        char* hash_ptr = (char*) (stk->data + stk->data_size); // ?
        HashDecrease (hash_ptr, &stk->hash_value, 0, sizeof (Elem_t));
    #endif

    /// The push itself.
    stk->data[stk->data_size++] = value;

    #ifdef HASH_PROTECTION
        stk->hash_value++;
        HashIncrease (hash_ptr, &stk->hash_value, 0, sizeof (Elem_t));

        StackHashError (stk);
    #endif

    return stk->stack_err;
}

ErrorType StackPop (Stack* const stk, Elem_t* const return_value)
{
    STACK_VERIFY (stk);

    if (INIT_CAPACITY <= stk->data_size &&
                         stk->data_size * RESIZE_MULTIPLIER * RESIZE_MULTIPLIER <=
                         stk->data_capacity)
    {
        stk->data_capacity /= RESIZE_MULTIPLIER;

        #ifdef CANARY_PROTECTION
            stk->data = (Elem_t*) ((Canary_t*)(void*) stk->data - 1);
        #endif

        StackDataAlloc(stk, (Elem_t*) stk->data);
    }

    #ifdef HASH_PROTECTION
        char* hash_ptr = (char*) (stk->data + stk->data_size - 1);
        HashDecrease (hash_ptr, &stk->hash_value, 0, sizeof (Elem_t));
    #endif

    /// The pop itself.
    *return_value = stk->data[--stk->data_size];
    stk->data[stk->data_size] = POISON;

    #ifdef HASH_PROTECTION
        stk->hash_value--;
        HashIncrease (hash_ptr, &stk->hash_value, 0, sizeof (Elem_t));

        StackHashError (stk);
    #endif

    return stk->stack_err;
}
