#ifndef STACK_ERRORS_H
#define STACK_ERRORS_H

#include <stdarg.h>
#include "const.h"

/// @brief This define is a wrapper for StackVerify(). It also calls for STACK_DUMP().
/// @param stk Pointer to stack.
#define STACK_VERIFY(stk)\
    assert (stk);\
    (stk)->stack_err.STACK_ERROR_OCCURED = StackVerify (stk);\
    if ((stk)->stack_err.STACK_ERROR_OCCURED)\
    {\
        return (stk)->stack_err;\
    }

/// @brief This function checks if pointer to stack data is nullptr.
/// @param stk Pointer to stack.
/// @return If data is nullptr, return STACK_DATA_NULL_POINTER, else return 0.
Error_t StackDataPtrError (Stack* const stk);

/// @brief This function checks if there were any errors by far.
/// @param stack_err Struct with errors.
/// @return Value of STACK_ERROR_OCCURED.
Error_t StackIsAnyError   (ErrorType* const stack_err);

/// @brief This is a verificator for stack. It checks whether it is legit or not.
/// @param stk Pointer to stack.
/// @return Calls for StackIsAnyError() and returns its value.
Error_t StackVerify       (Stack* const stk);

#endif
