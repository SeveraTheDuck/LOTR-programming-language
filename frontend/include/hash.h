#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include "stack.h"
#include "errors.h"
#include "const.h"

#ifdef HASH_PROTECTION

/// @brief This function appends bytes values between begin_index and end_index to hash.
/// @param ptr Pointer to the beginning of byte section.
/// @param hash_ptr Pointer to hash.
/// @param begin_index Beginning index.
/// @param border_index Ending index.
    void HashIncrease (char* ptr, Hash_t* hash_ptr,
                       size_t begin_index, size_t border_index);

/// @brief This function substracts bytes values between begin_index and end_index to hash.
/// @param ptr Pointer to the beginning of byte section.
/// @param hash_ptr Pointer to hash.
/// @param begin_index Beginning index.
/// @param border_index Ending index.
    void HashDecrease (char* ptr, Hash_t* hash_ptr,
                       size_t begin_index, size_t border_index);

/// @brief This function finds hash of the stack.
/// @param stk Pointer to stack.
/// @param hash_ptr Pointer to hash.
/// @return It returns stack_err struct type of ErrorType.
    ErrorType StackFindHash     (Stack*  const stk,
                                 Hash_t* const hash_ptr);

/// @brief This function finds hash of the stack data.
/// @param stk Pointer to stack.
/// @param hash_ptr Pointer to hash.
/// @return It returns stack_err struct type of ErrorType.
    ErrorType StackDataFindHash (Stack*  const stk,
                                 Hash_t* const hash_ptr);

/// @brief This function checks whether the hash value in stack struct is equal to new-counted.
/// @param stk Pointer to stack.
/// @return It returns stack_err struct type of ErrorType.
    ErrorType StackHashError (Stack* const stk);
#endif

#endif
