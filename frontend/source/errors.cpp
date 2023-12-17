#include "../include/stack.h"
#include "../include/errors.h"

Error_t StackDataPtrError (Stack* const stk)
{
    assert(stk);

    if (!stk->data)
    {
        stk->stack_err.STACK_ERROR_OCCURED     = 1;
        stk->stack_err.STACK_DATA_NULL_POINTER = 1;
        return stk->stack_err.STACK_DATA_NULL_POINTER;
    }

    return 0;
}

Error_t StackIsAnyError (ErrorType* const stack_err)
{
    assert(stack_err);

    if (*(Error_t*) stack_err)
    {
        stack_err->STACK_ERROR_OCCURED = 1;
    }

    return stack_err->STACK_ERROR_OCCURED;
}

Error_t StackVerify (Stack* const stk)
{
    assert (stk);

    if (!stk->data)
    {
        stk->stack_err.STACK_DATA_NULL_POINTER         = 1;
    }

    if (stk->data_size < 0)
    {
        stk->stack_err.STACK_SIZE_NOT_LEGIT_VALUE      = 1;
    }

    if (stk->data_capacity < 0)
    {
        stk->stack_err.STACK_CAPACITY_NOT_LEGIT_VALUE  = 1;
    }

    if (stk->data_capacity < stk->data_size)
    {
        stk->stack_err.STACK_SIZE_OUT_OF_RANGE         = 1;
    }

    #ifdef CANARY_PROTECTION
        if (stk->left_canary != CANARY_VALUE)
        {
            stk->stack_err.STACK_LEFT_CANARY_DAMAGED       = 1;
        }

        if (stk->right_canary != CANARY_VALUE)
        {
            stk->stack_err.STACK_RIGHT_CANARY_DAMAGED      = 1;
        }

        if (*((Canary_t*)(void*) stk->data - 1) != CANARY_VALUE)
        {
            stk->stack_err.STACK_DATA_LEFT_CANARY_DAMAGED  = 1;
        }

        if (*(Canary_t*)(void*) (stk->data + stk->data_capacity) != CANARY_VALUE)
        {
            stk->stack_err.STACK_DATA_RIGHT_CANARY_DAMAGED = 1;
        }
    #endif

    return StackIsAnyError (&stk->stack_err);
}
